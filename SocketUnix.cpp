/*
  NAME
  SocketUnix.cpp - Body file of the SocketUnix class.

  DESCRIPTION
  Simplify UNIX socket programming.
  简化UNIX本地Socket编程的类
*/

#include "SocketUnix.hpp"
#include "Logger.hpp"
#include <cstdio>
#include <cstddef>
#include <unistd.h>
#include <sys/socket.h>

// 构造函数
SocketUnix::SocketUnix(const std::string &domain)
{
  _domain = domain;
}

// 析构函数
SocketUnix::~SocketUnix()
{
  if( _serverMode && !_domain.empty()) {
    unlink(_domain.c_str());
  }
}

// 启动UNIX套接字服务
bool SocketUnix::start_server()
{
  if((_listen_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    LOG_ERROR_MSG("Cannot create the UNIX socket.");
    return false;
  }
  _server_addr.sun_family = AF_UNIX;
  strncpy(_server_addr.sun_path, _domain.c_str(), sizeof(_server_addr.sun_path) - 1);
  unlink(_domain.c_str());
  if(bind(_listen_fd, (struct sockaddr *) &_server_addr, sizeof(_server_addr)) < 0) {
    LOG_ERROR_MSG("Cannot bind the UNIX socket.");
    return false;
  }
  if(listen(_listen_fd, MAX_CONNECTION_NUMBER) < 0) {
    LOG_ERROR_MSG("Cannot listen the UNIX socket.");
    return false;
  }
  LOG_INFO_MSG("UNIX server socket started successful.");
  _serverMode = true;
  return true;
}

// 连接UNIX套接字服务
bool SocketUnix::connect_server()
{
  if(_serverMode) {
    LOG_ERROR_MSG("Cannot connect to the server in it's own process.");
    return false;
  }
  if((_client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    LOG_ERROR_MSG("Cannot creat the UNIX socket.");
    return false;
  }
  _server_addr.sun_family = AF_UNIX;
  strncpy(_server_addr.sun_path, _domain.c_str(), sizeof(_server_addr.sun_path) - 1);
  if(connect(_client_fd, (struct sockaddr*) &_server_addr, sizeof(_server_addr)) < 0) {
    LOG_ERROR_MSG("Cannot connect to the UNIX socket.");
    return false;
  }
  return true;
}

// 等待UNIX套接字客户端连接
bool SocketUnix::socket_accept()
{
  socklen_t len = sizeof(_client_addr);
  if((_client_fd = accept(_listen_fd, (struct sockaddr*) &_client_addr, &len)) < 0 ) {
    LOG_ERROR_MSG("Cannot accept connection to the Unix socket.");
    return false;
  }
  return true;
}
