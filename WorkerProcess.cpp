/*
  NAME
    WorkerProcess.hpp - Body file of the Worker Process class.

  DESCRIPTION
    Worker Process.
    工作进程类
*/

#include "WorkerProcess.hpp"
#include "Configure.hpp"
#include "Logger.hpp"
#include <cstring>
#include <iterator>
#include <unistd.h>
#include <sys/types.h>

WorkerProcess::WorkerProcess() { }

// 构造函数
WorkerProcess::WorkerProcess(std::string name, std::string command, std::string work_dir, bool auto_start)
{
  if(auto_start) {
    _pid = -1;
  } else {
    _pid = 0;
  }
  _name = name;
  _command = command;
  _work_dir = work_dir;
  std::istringstream iss(_command);
  std::string token;
  while(iss >> token) {
    _tokens.push_back(token);
  }
}

// 析构函数
WorkerProcess::~WorkerProcess() { }

// 启动工作进程
bool WorkerProcess::launch() {
  if(_pid < 0) {
    const char **argv = new const char*[_tokens.size() + 1];   // execvp()函数最后的一个参数需要是NULL
    for (std::vector<std::string>::size_type i = 0; i < _tokens.size() + 1; ++i) {
      argv[i] = _tokens[i].c_str();
    }
    argv[_tokens.size()] = NULL;

    LOG_DEBUG_MSG("Starting worker:[" + _name + "] process using command:[" + _command + "].");
    _pid = fork();
    switch(_pid) {
      case -1:  // 创建新进程失败
        LOG_ERROR_MSG("Error creating child process using fork.");
        break;
      case 0:   // 子进程
        Logger::stop();
        // 关闭无效的文件句柄，父进程打开了一个UNIX socket
        for(int x = sysconf(_SC_OPEN_MAX); x > 0; x--) {
          close(x);
        }
        Logger::start(Configure::get_log_dir() + _name + ".out");
        if(chdir(Configure::get_work_dir().c_str()) != 0) {
          LOG_ERROR_MSG("Cannot change to the daemon work directory.");
        }
        if(!_work_dir.empty()) {
          if(chdir(_work_dir.c_str()) != 0) {
            LOG_ERROR_MSG("Cannot change to the work directory:[." + _work_dir + "].");
          }
        }
        if(execvp(argv[0], (char**) argv) < 0) {
          LOG_ERROR_MSG("Error executing command:[" + _command + "].");
          _pid = 0;
          exit(EXIT_FAILURE);
        }
        break;
      default:  // 父进程
        LOG_INFO_MSG("Worker:[" + _name + "] process with command:[" + _command + "] now running PID:[" + std::to_string(_pid) + "].");
    }
  }
  return (_pid > 0);
}

// 如果工作进程原PID为old_pid,需要重启动它
void WorkerProcess::relaunch(int old_pid) {
    if(_pid == old_pid) {
      _pid = -1;
      LOG_DEBUG_MSG("Worker process with PID:[" + std::to_string(old_pid) + "] will be relaunch.");
    }
}

// 获取工作进程当前的PID
pid_t WorkerProcess::get_pid()
{
  return _pid;
}

// 设置工作进程的PID
void WorkerProcess::set_pid(pid_t pid)
{
  _pid = pid;
}
