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

void usage()
{
  std::cout << "Run this program with arguments which to send to the PSMON daemon!" << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "  pscli reload" << std::endl;
  std::cout << "    Reload all configuration, process must be stopped before deleting." << std::endl;
  std::cout << "  pscli status" << std::endl;
  std::cout << "    Show status of all configured process." << std::endl;
  std::cout << "  pscli start hello" << std::endl;
  std::cout << "    Start process named 'hello', change it to the configured process name." << std::endl;
  std::cout << "  pscli stop hello" << std::endl;
  std::cout << "    Stop process named 'hello', change it to the configured process name." << std::endl;
  std::cout << "  pscli restart hello" << std::endl;
  std::cout << "    Restart process named 'hello', change it to the configured process name." << std::endl;
}

int main(int argc, char** argv)
{
  switch(argc) {
    case 1:
      usage();
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