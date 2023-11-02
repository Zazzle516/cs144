#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  cerr << "Warning: get_URL() has not been implemented yet.\n";
  // new tcpsocket 对象
  TCPSocket tcp_socket = TCPSocket();

  string buffer;

  // 0-Construct by resolving a hostname and servicename
  Address address(host,"http");

  // 建立和 host + path 的连接 1-客户端发出请求 2-服务端接收并返回请求 3-客户端同意 连接建立 一个函数搞定
  tcp_socket.connect(address);
  
  // 写入 linux 命令
  tcp_socket.write("GET " + path + " HTTP/1.1\r\n");
  tcp_socket.write("Host: " + host + "\r\n\r\n");
  tcp_socket.write("Connection: close\r\n\r\n");

  // 4-通知服务器结束连接 `SHUT_RD`, `SHUT_WR`, or `SHUT_RDWR`
  tcp_socket.shutdown( SHUT_WR );

  // 如果文件还没有结束就持续写入 file_descriptor.cc line_83
  while(!tcp_socket.eof()){
    tcp_socket.read(buffer);
    cout << buffer;
  }

  // 5-理论上来说不需要的
  // tcp_socket.close();
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
