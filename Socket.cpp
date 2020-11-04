/*
  NAME
  Socket.cpp - Body file of the Socket class.

  DESCRIPTION
  Simplify socket programming.
  简化Socket编程的类
*/

#include "Socket.hpp"
#include "Logger.hpp"
#include <cstring>
#include <unistd.h>

char Socket::_buffer[1024];

// 构造函数
Socket::Socket()
{
  _client_fd = -1;
  _listen_fd = -1;
  _serverMode = false;
}

// 析构函数
Socket::~Socket()
{
  if(_client_fd > 0) {
    close(_client_fd);
  }
  if(_listen_fd > 0) {
    close(_listen_fd);
  }
}

int Socket::read_message(std::string &message)
{
  if(_client_fd > 0) {
    memset(_buffer, 0, sizeof(_buffer));
    int bytes = read(_client_fd, _buffer, sizeof(_buffer));
    message.assign(_buffer);
    return bytes;
  }
  return 0;
}

int Socket::write_message(const std::string &message)
{
  long unsigned len = message.length();
  if(len >= sizeof(_buffer)) {
    LOG_ERROR_MSG("The length of message is too large.");
    return 0;
  }
  if(_client_fd > 0) {
    memset(_buffer, 0, sizeof(_buffer));
    strncpy(_buffer, message.c_str(), len);
    int bytes = write(_client_fd, _buffer, len);
    return bytes;
  }
  return 0;
}

void Socket::close_client()
{
  if(_client_fd > 0) {
    close(_client_fd);
  }
}
