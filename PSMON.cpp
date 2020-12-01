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
  std::cout << "\t\t Start psmon as a daemon service without arguments." << std::endl;
  std::cout << "\t psmon -d" << std::endl;
  std::cout << "\t psmon --daemon" << std::endl;
  std::cout << "\t\t Running psomn as a daemon service." << std::endl;
  std::cout << "\t psmon -h" << std::endl;
  std::cout << "\t psmon --help" << std::endl;
  std::cout << "\t\t Echo this help message." << std::endl;
  std::cout << "\t psmon -s" << std::endl;
  std::cout << "\t psmon --delete_semaphore" << std::endl;
  std::cout << "\t\t Delete semaphore. Make sure psomn is not running!" << std::endl;
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
    // 无参数，以普通模式启动程序。适合用systemd配置为服务
    daemon = PsmonDaemon::instance(daemon_name, false);
    if(NULL != daemon) {
      daemon->run();
      PsmonDaemon::destory();
    }
    break;
  case 2:
    if(0 == strcmp("--daemon", argv[1]) || 0 == strcmp("-d", argv[1])) {
      // 以守护进程的方式启动程序
      daemon = PsmonDaemon::instance(daemon_name, true);
      if(NULL != daemon) {
        daemon->run();
        PsmonDaemon::destory();
      }
    } else if(0 == strcmp("--delete_semaphore", argv[1]) || 0 == strcmp("-s", argv[1])) {
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
