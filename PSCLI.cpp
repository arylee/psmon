/*
  NAME
    PSCLI.cpp - Main file of client for processing monitor.

  DESCRIPTION
    Client for Processing Monitor
    进程监控的客户端主程序
*/

#include "Configure.hpp"
#include "SocketUnix.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char** argv)
{
  switch(argc) {
    case 1:
      std::cout << "Run this program with arguments which to send to the PSMON daemon!" << std::endl;
      break;
    default:
      SocketUnix socket(Configure::get_home_dir() + UNIX_DOMAIN);
      if (socket.connect_server()) {
        std::string arguments = "cmd:";
        for(int i = 1; i < argc; i++) {
          arguments += argv[i];
          if(i < argc - 1) {
            arguments += ' ';
          }
        }
        int bytes = socket.write_message(arguments);
        if(bytes > 0) {
          std::string msg;
          bytes = socket.read_message(msg);
          if(bytes > 0) {
            std::cout << msg << std::endl;
          } else {
            std::cout << "Error reading from UNIX Socket." << std::endl;
          }
        } else {
          std::cout << "Error writing to UNIX Socket." << std::endl;
        }
      } else {
        std::cout << "Cannot connect to the UNIX Socket, maybe PSMON is not running." << std::endl;
      }
  }
}