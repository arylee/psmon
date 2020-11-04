/*
  NAME
  Socket.hpp - Header file of the Socket class.

  DESCRIPTION
  Simplify socket programming.
*/

#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>

class Socket {
  public:
    Socket();
    ~Socket();

    virtual bool start_server() = 0;
    virtual bool connect_server() = 0;
    virtual bool socket_accept() = 0;
    int read_message(std::string &message);
    int write_message(const std::string &message);
    void close_client();
  protected:
    bool _serverMode;
    int _client_fd;
    int _listen_fd;
  private:
    static char _buffer[1024];
};

#endif
