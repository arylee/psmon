/*
  NAME
    BaseDaemon.cpp - Body file of the base daemon class.

  DESCRIPTION
    Simplify daemon program.
    服务进程
*/

#include "Configure.hpp"
#include "BaseDaemon.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// 服务运行标记
bool BaseDaemon::running = false;

// 信号处理函数
void terminate_handle(int sig)
{
  LOG_INFO_MSG("Got SIG_TERM signal, will ignore.");
}

// 切换为服务进程
void BaseDaemon::skeleton_daemon()
{
  pid_t pid;
  // 第一次fork
  pid = fork();
  // 创建新进程失败
  if(pid < 0) {
    exit(EXIT_FAILURE);
  }
  // 结束父进程
  if(pid > 0) {
    exit(EXIT_SUCCESS);
  }
  // 设置子进程SID,子进程获得会话信息
  if(setsid() < 0) {
    exit(EXIT_FAILURE);
  }
  // 第二次fork
  pid = fork();
  // 创建新进程失败
  if(pid < 0) {
    exit(EXIT_FAILURE);
  }
  // 结束父进程
  if(pid > 0) {
    exit(EXIT_SUCCESS);
  }
  // 设置信号处理
  signal(SIGHUP, SIG_IGN);
  signal(SIGTERM, terminate_handle);
  // 子进程被init接管，脱离会话成为服务进程
  // 设置umask文件权限
  umask(0);
  // 改变当前的工作目录
  if(chdir(Configure::get_home_dir().c_str()) != 0) {
    LOG_ERROR_MSG("Cannot change to the daemon home directory.");
  }

  // 关闭无效的文件句柄：stdin, stdout, stderr
  for(int x = sysconf(_SC_OPEN_MAX); x > 0; x--) {
    close(x);
  }
}

// 构造函数
BaseDaemon::BaseDaemon()
{
  _configure = NULL;
  running = true;
  _sleep_interval = 60;
}

// 析构函数
BaseDaemon::~BaseDaemon()
{
  LOG_TRACE_MSG("Destructor of base daemon...");
  Logger::stop();
  if(_configure) {
    Configure::destory();
    _configure = NULL;
  }
}

// 启动服务
void BaseDaemon::run()
{
  if(init()) {
    if(path_exist(Configure::get_home_dir())) {
      _configure = Configure::instance(_daemon_name + ".conf");
    } else {
      LOG_ERROR_MSG("Cannot access daemon home directory, maybe environment variable not set.");
    }
    if(_configure && _configure->configured()) {
      skeleton_daemon();
      Logger::start(Configure::get_log_dir() + _daemon_name + ".log");
      LOG_INFO_MSG("The daemon program:[" + _daemon_name +"] started.");
      _sleep_interval = _configure->get_sleep_interval();
      if(prepare()) {
        while(running) {
          process();
          sleep(_sleep_interval);
        }
      }
      LOG_INFO_MSG("The daemon program:[" + _daemon_name +"] stoped.");
    } else {
      LOG_ERROR_MSG("Cannot access configure file of daemon:[" + _daemon_name + "].");
    }
  }
}
