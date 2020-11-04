/*
  NAME
  SocketUnix.hpp - Header file of the SocketUnix class.

  DESCRIPTION
  Simplify UNIX socket programming.
*/

#ifndef __SOCKET_UNIX_HPP__
#define __SOCKET_UNIX_HPP__

#define MAX_CONNECTION_NUMBER 1

#include "Socket.hpp"
#include <string>
#include <sys/un.h>

class SocketUnix : public Socket {
  public:
    SocketUnix(const std::string &domain);
    virtual ~SocketUnix();
    virtual bool start_server();
    virtual bool connect_server();
    virtual bool socket_accept();
  private:
    std::string _domain;

    struct sockaddr_un _client_addr;
    struct sockaddr_un _server_addr;
};

#endif
