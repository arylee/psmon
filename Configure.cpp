/*
  NAME
    Configure.cpp - Body file of the Configure class.

  DESCRIPTION
    Wrapper class for getting configuration.
    配置信息类
*/

#include "Configure.hpp"
#include "Logger.hpp"
#include "Symbol.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <unistd.h>

// 单态指针
Configure* Configure::_instance = NULL;

std::string Configure::_conf_dir;
std::string Configure::_data_dir;
std::string Configure::_home_dir;
std::string Configure::_log_dir;
std::string Configure::_work_dir;

// 创建单态实例
Configure* Configure::instance(const std::string& config_filename)
{
  if(!_instance) {
    _instance = new Configure(config_filename);
  }
  return _instance;
}

// 销毁单态实例
void Configure::destory()
{
  if(_instance) {
    delete _instance;
    _instance = NULL;
  }
}

// 获取配置目录
std::string Configure::get_conf_dir()
{
  if("" == _conf_dir) {
    char* conf_dir = NULL;
    conf_dir = getenv(ENV_CONF);
    if(conf_dir) {
      _conf_dir = conf_dir;
    } else {
      _conf_dir = get_home_dir() + S_CONF;
    }
    if(C_SLASH != _conf_dir[_conf_dir.size() - 1]) {
      _conf_dir.append(S_SLASH);
    }
  }
  return _conf_dir;
}

// 获取数据目录
std::string Configure::get_data_dir()
{
  if("" == _data_dir) {
    char* data_dir = NULL;
    data_dir = getenv(ENV_DATA);
    if(data_dir) {
      _data_dir = data_dir;
    } else {
      _data_dir = get_home_dir() + S_DATA;
    }
    if(C_SLASH != _data_dir[_data_dir.size() - 1]) {
      _data_dir.append(S_SLASH);
    }
  }
  return _data_dir;
}

// 获取应用程序主目录
std::string Configure::get_home_dir()
{
  if("" == _home_dir) {
    char* home_dir = NULL;
    home_dir = getenv(ENV_HOME);
    if(home_dir) {
      _home_dir = home_dir;
    } else {
      _home_dir = S_HOME;
    }
    if(C_SLASH != _home_dir[_home_dir.size() - 1]) {
      _home_dir.append(S_SLASH);
    }
  }
  return _home_dir;
}

// 获取日志目录
std::string Configure::get_log_dir()
{
  if("" == _log_dir) {
    char* log_dir = NULL;
    log_dir = getenv(ENV_LOG);
    if(log_dir) {
      _log_dir = log_dir;
    } else {
      _log_dir = get_home_dir() + S_LOG;
    }
    if(C_SLASH != _log_dir[_log_dir.size() - 1]) {
      _log_dir.append(S_SLASH);
    }
  }
  return _log_dir;
}

// 获取服务进程工作目录
std::string Configure::get_work_dir()
{
  if("" == _work_dir) {
    char* work_dir = NULL;
    work_dir = getenv(ENV_WORK);
    if(work_dir) {
      _work_dir = work_dir;
    } else {
      _work_dir = get_home_dir() + S_WORK;
    }
    if(C_SLASH != _work_dir[_work_dir.size() - 1]) {
      _work_dir.append(S_SLASH);
    }
  }
  return _work_dir;
}

// 构造函数
Configure::Configure(const std::string& config_filename)
{
  _config_file = NULL;
  _sleep_interval = 0;
  std::string filename(get_conf_dir() + C_DOT + config_filename);
  filename = get_conf_dir() + config_filename;
  if(file_exist(filename)) {
    _config_file = new ConfigFile(filename);
  }
}

// 析构函数
Configure::~Configure()
{
  if(_config_file) {
    delete _config_file;
    _config_file = NULL;
  }
}

// 判断配置文件是否存在
bool Configure::configured()
{
  return _config_file;
}

// 获取服务休眠秒数
unsigned int Configure::get_sleep_interval()
{
  if(0 == _sleep_interval) {
    _sleep_interval = 60;
    if(_config_file) {
      try {
        _sleep_interval = atoi(_config_file->value(S_DAEMON, S_SLEEP_INTL).c_str());
        if(_sleep_interval < 5 || _sleep_interval > 1800) {
          _sleep_interval = 60;
          LOG_WARN_MSG("Sleep interval should between 5 and 1800, using default value:[60].");
        }
      } catch(const char* msg) {
        LOG_WARN_MSG("Sleep interval not configured, using default sleep interval:[60].");
      }
    }
  }
  return _sleep_interval;
}

// 获取配置文件名
std::string Configure::get_conf_filename()
{
  return _config_file->value(S_DAEMON, S_CONF_NAME);
}

// 获取自动启动应用的配置信息
bool Configure::get_auto_start()
{
  try {
    return atoi(_config_file->value(S_DAEMON, S_AUTO_START).c_str()) > 0;
  } catch(const char* msg) {
    return true;
  }
}
