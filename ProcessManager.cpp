/*
  NAME
    ProcessManager.hpp - Body file of the Process Manager class.

  DESCRIPTION
    Process Manager.
    进程管理器类
*/

#include "ProcessManager.hpp"
#include "BaseDaemon.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <iterator>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 单态指针
ProcessManager* ProcessManager::_instance;

// 创建单态实例
ProcessManager* ProcessManager::instance(std::string filename) 
{
  if(!_instance) {
    _instance = new ProcessManager(filename);
  }
  return _instance;
}

// 释放单态实例
void ProcessManager::destory()
{
  if(_instance) {
    delete _instance;
    _instance = NULL;
  }
}

// 构造函数
ProcessManager::ProcessManager(std::string filename)
{
  _filename = filename;
  // 创建线程锁，并初始化
  _mutex = new pthread_mutex_t();
  pthread_mutex_init(_mutex, NULL);
  // 读取工作进程配置文件
  if(file_exist(filename)) {
    _config_file = new ConfigFile(filename);
  } else {
    LOG_ERROR_MSG("Cannot access configure file:[" + filename + "].");
  }
  if(_mutex && _config_file) {
    std::list<std::string>* sections = _config_file->get_sections();
    // 锁定线程锁，STL中的map并不是线程安全的
    pthread_mutex_lock(_mutex);
    std::list<std::string>::const_iterator ci = sections->begin();
    while(ci != sections->end()) {
      std::string section = *ci++;
      std::string command = _config_file->value(section, S_COMMAND);
      std::string work_dir;
      LOG_DEBUG_MSG("Found section:[" + section + "], command:[" + command + "].");
      try {
        work_dir = _config_file->value(section, S_WORK_DIR);
        LOG_INFO_MSG("Work process:[" + section + "] will use self defined work dir:[" + work_dir + "].");
      } catch(const char* msg) {
        LOG_INFO_MSG("Work process:[" + section + "] will use default daemon work dir:[" + Configure::get_work_dir() + "].");
      }
      bool auto_start = true;
      try {
        auto_start = atoi(_config_file->value(section, S_AUTO_START).c_str()) > 0;
      } catch(const char* msg) { }
      _processes[section] = WorkerProcess(section, command, work_dir, auto_start);
    }
    // 释放线程锁，STL中的map并不是线程安全的
    pthread_mutex_unlock(_mutex);
  }
}

// 析构函数
ProcessManager::~ProcessManager()
{
  LOG_TRACE_MSG("Destructor of ProcessManager.");
  // 销毁线程锁
  if(_mutex) {
    pthread_mutex_destroy(_mutex);
    delete _mutex;
    _mutex = NULL;
  }
  if(_config_file) {
    delete _config_file;
    _config_file = NULL;
  }
}

// 启动指定名称的Worker工作进程
std::string ProcessManager::start(std:: string name)
{
  std::string msg = S_NO_WORKER;
  // 锁定线程锁，STL中的map并不是线程安全的
  pthread_mutex_lock(_mutex);
  std::map<std::string, WorkerProcess>::iterator it = _processes.find(name); 
  if(it != _processes.end()) {    
    pid_t pid = it->second.get_pid();
    if(pid > 0) {
      LOG_INFO_MSG("Worker process:[" + name + "] already started.");
      msg = S_SKIP;
    } else {
      LOG_INFO_MSG("Work process:[" + name + "] will start.");
      it->second.set_pid(-1);
      if(it->second.launch()) {
        msg = S_OK;
      } else {
        msg = S_ERROR;
      }
    }
  }
  // 释放线程锁，STL中的map并不是线程安全的
  pthread_mutex_unlock(_mutex);
  return msg;
}

// 停止指定名称的Worker工作进程
std::string ProcessManager::stop(std:: string name)
{
  std::string msg = S_NO_WORKER;
  // 锁定线程锁，STL中的map并不是线程安全的
  pthread_mutex_lock(_mutex);
  std::map<std::string, WorkerProcess>::iterator it = _processes.find(name);
  if(it != _processes.end()) {
    pid_t pid = it->second.get_pid();
    if(pid > 0) {
      LOG_INFO_MSG("Work process:[" + name + "] will stop.");
      if(kill(pid, SIGTERM)) {
        LOG_ERROR_MSG("Failed to send SIGTERM to worker process:[" + std::to_string(pid) + "].");
        msg = S_ERROR;
      } else {
        it->second.set_pid(0);
        LOG_INFO_MSG("Send SIGTERM to worker process:[" + std::to_string(pid) + "] successful.");
        msg = S_OK;
      }
      bool needKill = true;
      for(int i = 0; i < 3; i++) {
        if(!kill(pid, 0)) {
          needKill = false;
          break;
        }
        sleep(1);
      }
      if(needKill) {
        if(kill(pid, SIGKILL)) {
          LOG_ERROR_MSG("Failed to send SIGKILL to worker process:[" + std::to_string(pid) + "].");
          msg = S_ERROR;
        } else {
          it->second.set_pid(0);
          LOG_INFO_MSG("Send SIGKILL to worker process:[" + std::to_string(pid) + "] successful.");
          msg = S_OK;
        }
      }
    } else {
      LOG_INFO_MSG("Worker process:[" + name + "] already stopped.");
      msg = S_SKIP;
    }
  }
  // 释放线程锁，STL中的map并不是线程安全的
  pthread_mutex_unlock(_mutex);
  return msg;
}

// 获取所有Worker工作进程的状态
std::string ProcessManager::status()
{
  std::string msg;
  // 锁定线程锁，STL中的map并不是线程安全的
  pthread_mutex_lock(_mutex);
  for(std::map<std::string, WorkerProcess>::iterator it = _processes.begin(); it != _processes.end(); it++) {
    msg += it->first;
    msg += ':';
    msg += std::to_string(it->second.get_pid());
    msg += ' ';
  }
  // 释放线程锁，STL中的map并不是线程安全的
  pthread_mutex_unlock(_mutex);
  return msg;
}

// 启动所有的工作进程
void ProcessManager::start_worker_processes()
{
  // 锁定线程锁，STL中的map并不是线程安全的
  pthread_mutex_lock(_mutex);
  for(std::map<std::string, WorkerProcess>::iterator it = _processes.begin(); it != _processes.end(); it++) {
    it->second.launch();
  }
  // 释放线程锁，STL中的map并不是线程安全的
  pthread_mutex_unlock(_mutex);
  // 检查子进程结束并回收，防止子进程变成僵尸进程
  int status = 0;
  pid_t pid;
  pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
  if (pid > 0) {
    bool should_relaunch = false;
    if(WIFEXITED(status)) {
      // 进程正常退出
      int ret = WEXITSTATUS(status);
      LOG_INFO_MSG("Worker process with PID:[" + std::to_string(pid) + "] returned code:[" + std::to_string(ret) + "].");
      should_relaunch = true;
    }
    if(WIFSIGNALED(status)) {
      // 被信号中断退出
      if(WCOREDUMP(status)) {
        LOG_ERROR_MSG("Worker process with PID:[" + std::to_string(pid) + "] cause core dump.");
      } else {
        LOG_WARN_MSG("Worker process with PID:[" + std::to_string(pid) + "] terminated by signal:[" + std::to_string(WTERMSIG(status)) + "].");
      }
      should_relaunch = true;
    }
    if(WIFSTOPPED(status)) {
      // 被信号中断停止
      LOG_INFO_MSG("Worker process with PID:[" + std::to_string(pid) + "] stopped by signal:[" + std::to_string(WSTOPSIG(status)) + "].");
    }
    if(WIFCONTINUED(status)) {
      // 收到SIGCONT信号，继续运行
      LOG_INFO_MSG("Worker process with PID:[" + std::to_string(pid) + "] continued.");
    }
    if(should_relaunch) {
      relaunch(pid);
    }
  }
}

// 如果工作进程原PID为old_pid,需要重启动它
void ProcessManager::relaunch(int old_pid)
{
  // 锁定线程锁，STL中的map并不是线程安全的
  pthread_mutex_lock(_mutex);
  for(std::map<std::string, WorkerProcess>::iterator it = _processes.begin(); it != _processes.end(); it++) {
    it->second.relaunch(old_pid);
  }
  // 释放线程锁，STL中的map并不是线程安全的
  pthread_mutex_unlock(_mutex);
}

// 重新加载配置文件
std::string ProcessManager::reload()
{
  if(file_exist(_filename)) {
    // 释放之前已经加载的文件
    if(_config_file) {
      delete _config_file;
    _config_file = NULL;
    }
    _config_file = new ConfigFile(_filename);
    // 锁定线程锁，STL中的map并不是线程安全的
    pthread_mutex_lock(_mutex);
    // 删除已经停止的，未在配置文件中的工作线程
    for(std::map<std::string, WorkerProcess>::iterator it = _processes.begin(); it != _processes.end(); ) {
      if(!_config_file->has_section(it->first)) {
        int pid = it->second.get_pid();
        if(pid > 0) {
          LOG_INFO_MSG("Worker process:[" + it->first + "] are still running with PID:[" + std::to_string(pid) + "].");
        } else {
          _processes.erase(it++);
          continue;
        }
      }
      it++;
    }
    // 新增配置文件中加入的工作线程
    std::list<std::string>* sections = _config_file->get_sections();
    std::list<std::string>::const_iterator ci = sections->begin();
    while(ci != sections->end()) {
      std::string section = *ci++;
      std::map<std::string, WorkerProcess>::const_iterator ci = _processes.find(section);
      if(_processes.end() == ci) {
        std::string command = _config_file->value(section, S_COMMAND);
        std::string work_dir;
        LOG_DEBUG_MSG("Found section:[" + section + "], command:[" + command + "].");
        try {
          work_dir = _config_file->value(section, S_WORK_DIR);
          LOG_INFO_MSG("Worker process:[" + section + "] will use self defined work dir:[" + work_dir + "].");
        } catch(const char* msg) {
          LOG_INFO_MSG("Worker process:[" + section + "] will use default daemon work dir:[" + Configure::get_work_dir() + "].");
        }
        bool auto_start = true;
        try {
          auto_start = atoi(_config_file->value(section, S_AUTO_START).c_str()) > 0;
        } catch(const char* msg) { }
        _processes[section] = WorkerProcess(section, command, work_dir, auto_start);
      }
    }
    // 释放线程锁，STL中的map并不是线程安全的
    pthread_mutex_unlock(_mutex);
    return S_OK;
  } else {
    return S_ERROR;
  }
}

