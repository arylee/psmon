/*
  NAME
    PsmonDaemon.hpp - Body file of the PSMON daemon class.

  DESCRIPTION
    Processing monitor daemon program.
    进程监控守护程序
*/

#include "Configure.hpp"
#include "PsmonDaemon.hpp"
#include "SocketUnix.hpp"
#include "Symbol.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>

const std::string VERSION_INFO("0.1.1.18");

const std::string BUILD_DATETIME("2020-12-01 10:00:00");

const std::string VERSION_MESSAGE("Version:[" + VERSION_INFO + "], last build datetime:[" + BUILD_DATETIME + "].");

SocketUnix* unixSocket = NULL;

// 处理UNIX套接字的通讯信息
void* process_unix_socket(void* p)
{
  std::string request;
  std::string response;
  pthread_detach(pthread_self());
  while(unixSocket && BaseDaemon::running) {
    if(unixSocket->socket_accept()) {
      int bytes = unixSocket->read_message(request);
      if(bytes > 0) {
        LOG_TRACE_MSG("Got UNIX socket message:[" + request + "].");
        std::vector<std::string> tokens;
        std::istringstream iss(request);
        std::string token;
        while(iss >> token) {
          tokens.push_back(token);
        }
        std::string msg;
        switch(tokens.size()) {
          case 1:
            if(0 == tokens[0].compare(S_CMD_STATUS)) {
              msg = ((ProcessManager*) p)->status();
            } else {
              msg = S_SYNTAX_ERROR;
            }
            break;
          case 2:
            if(0 == tokens[0].compare(S_CMD_START)) {
              msg = ((ProcessManager*) p)->start(tokens[1]);
            } else if(0 == tokens[0].compare(S_CMD_STOP)) {
              msg = ((ProcessManager*) p)->stop(tokens[1]);
            } else {
              msg = S_SYNTAX_ERROR;
            }
            break;
          default:
            msg = S_SYNTAX_ERROR;
        }
        unixSocket->write_message(msg);
      }
      unixSocket->close_client();
      sleep(1);
    }
  }
  return NULL;
}

// 防止服务重复启动的信号灯
sem_t* PsmonDaemon::_semaphore = NULL;

// 单态指针
PsmonDaemon* PsmonDaemon::_instance = NULL;

// 创建单态实例
PsmonDaemon* PsmonDaemon::instance(const std::string& daemon_name, bool daemon_mode)
{
  if(!_instance) {
    _instance = new PsmonDaemon(daemon_name, daemon_mode);
  }
  return _instance;
}

// 销毁单态实例
void PsmonDaemon::destory()
{
  if(_instance) {
    delete _instance;
    _instance = NULL;
  }
}

// 显示版本信息
void PsmonDaemon::version()
{
  LOG_INFO_MSG(VERSION_MESSAGE);
}

// 构造函数
PsmonDaemon::PsmonDaemon(const std::string& daemon_name, bool daemon_mode)
{
  daemon = daemon_mode;
  _daemon_name = daemon_name;
}

// 析构函数
PsmonDaemon::~PsmonDaemon()
{
  LOG_TRACE_MSG("Destructor of psmon daemon...");
  if(_semaphore) {
    sem_close(_semaphore);
    sem_unlink(_daemon_name.c_str());
    _semaphore = NULL;
  }
  if(unixSocket) {
    delete unixSocket;
    unixSocket = NULL;
  }
  if(_ps_man) {
    ProcessManager::destory();
    _ps_man = NULL;
  }
}

// 初始化
bool PsmonDaemon::init()
{
  _semaphore = sem_open(_daemon_name.c_str(), O_CREAT | O_EXCL, 0644, 0);
  if(SEM_FAILED == _semaphore) {
    LOG_ERROR_MSG("Cannot create semaphore, maybe the daemon already started.");
    return false;
  }
  return true;
}

// 准备处理
bool PsmonDaemon::prepare()
{
  _ps_man = ProcessManager::instance(_configure->get_conf_dir() + _configure->get_conf_filename());
  if(NULL == _ps_man) {
    LOG_ERROR_MSG("Cannot instance class ProcessingManager, maybe not enough memory.");
    return false;
  }
  LOG_INFO_MSG(VERSION_MESSAGE);
  if(!unixSocket) {
    unixSocket = new SocketUnix(UNIX_DOMAIN);
  }
  bool result = unixSocket->start_server();
  pthread_t thread;
  pthread_create(&thread, NULL, process_unix_socket, _ps_man);
  return result;
}

// 处理任务
void PsmonDaemon::process()
{
  _ps_man->start_worker_processes();
}
