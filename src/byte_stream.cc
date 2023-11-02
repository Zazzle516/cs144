#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  // Push data to stream, but only as much as available capacity allows
  total_write = data.size();
  // 如果待写入的数据大小为 0
  if(total_write == 0)
    return ;
  else{
    // 如果想写入的数据超过了当前的容量
    if(total_write > available_capacity()){
      uint64_t len = available_capacity();
      data.resize(len);
    }

    // 调整到合适的大小然后写入
    buffer_data.push(move(data));
    if(buffer_data.size() == 1){
      buffer_peek = buffer_data.front();
    }
    nr_write += min(total_write,available_capacity());

    // 对应截图 3- 的错误，设计内存访问 猜测是这里的问题 解决了
    // buffer_peek 读取数据的单位 字节
  }

  // 如果已经写回的数据超出理论这时出错
  if( nr_write > total_write ){
    set_error();
    return ;
  }
  return ;
}

void Writer::close()
{
  // Signal that the stream has reached its ending. Nothing more will be written
  if(nr_write == total_write){
    is_eof_write = true;
  }
  return ;
}

void Writer::set_error()
{
  //  Signal that the stream suffered an error
  is_error = true;
  return ;
}

bool Writer::is_closed() const
{
  // Has the stream been closed?
  if( is_eof_write )
    return true;
  return false;
}

uint64_t Writer::available_capacity() const
{
  // How many bytes can be pushed to the stream right now
  // 要考虑到 writer 与 reader 的交互 
  return capacity_ - nr_write + reader().bytes_popped();
}

uint64_t Writer::bytes_pushed() const
{
  // Total number of bytes cumulatively pushed to the stream
  return nr_write;
}


string_view Reader::peek() const
{
  // Peek at the next bytes in the buffer
  // writer().push
  return buffer_peek;
}

bool Reader::is_finished() const
{
  // Is the stream finished (closed and fully popped)
  // 1-不会继续往里面写 2-数据也都被读完了 / nr_read == nr_write
  if(writer().is_closed() && buffer_data.size() == 0)
    return true;
  return false;
}

bool Reader::has_error() const
{
  // Has the stream had an error
  if(is_error)
    return true;
  return false;
}

void Reader::pop( uint64_t len )
{
  // Remove `len` bytes from the buffer
  // 此处的判断不能用 nr_write 针对当前缓存区的判断而不是写入总计 bytes_buffered
  if(!len || len > bytes_buffered() )
    return ;
  // 每次读出一个 buffer_peek(byte) 的大小
  while(len){
    if(len >= buffer_peek.size()){
      len -= buffer_peek.size();
      buffer_data.pop();
      nr_read += 1;
      buffer_peek = buffer_data.front();
    }
    // 那这个 else 的判断有什么意义吗，只是在处理 bit 而已
    else{
      buffer_peek.remove_prefix(len);
      len = 0;
    }
  }
  return ;
}

uint64_t Reader::bytes_buffered() const
{
  // Number of bytes currently buffered (pushed and not popped) 当前
  return writer().bytes_pushed() - bytes_popped();
}

uint64_t Reader::bytes_popped() const
{
  // Total number of bytes cumulatively popped from stream
  return nr_read;
}
