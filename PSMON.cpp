/*
  NAME
    PSMON.cpp - Main file of processing monitor.

  DESCRIPTION
    Processing Monitor
    进程监控的主程序
*/

#include "PsmonDaemon.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <semaphore.h>

void usage()
{
  std::cout << "Usage:" << std::endl;
  std::cout << "\t psmon" << std::endl;
  std::cout << "\t\t Start psmon as a daemon service without arguments."
             << std::endl;
  std::cout << "\t psmon -h" << std::endl;
  std::cout << "\t psmon --help" << std::endl;
  std::cout << "\t\t Echo this help message." << std::endl;
  std::cout << "\t psmon -d" << std::endl;
  std::cout << "\t psmon --delete_semaphore" << std::endl;
  std::cout << "\t\t Delete semaphore. Make sure no psomn daemon service is running!"
            << std::endl;
  std::cout << "\t psmon -v" << std::endl;
  std::cout << "\t psmon --version" << std::endl;
  std::cout << "\t\t Display the virsion information." << std::endl << std::endl;
}

int main(int argc, char** argv)
{
  const std::string daemon_name("PSMON");
  PsmonDaemon* daemon = NULL;
  switch(argc) {
  case 1:
    // 无参数，启动服务
    daemon = PsmonDaemon::instance(daemon_name);
    if(NULL != daemon) {
      daemon->run();
      PsmonDaemon::destory();
    }
    break;
  case 2:
    if(0 == strcmp("--delete_semaphore", argv[1]) || 0 == strcmp("-d", argv[1])) {
      // 删除信号灯
      sem_unlink(daemon_name.c_str());
      std::cout << "Semaphore was deleted successful." << std::endl;
    } else if(0 == strcmp("--version", argv[1]) || 0 == strcmp("-v", argv[1])) {
      // 显示版本信息
      PsmonDaemon::version();
    } else if(0 == strcmp("--help", argv[1]) || 0 == strcmp("-h", argv[1])) {
      // 使用帮助
      usage();
    }
    break;
  }
  return EXIT_SUCCESS;
}
