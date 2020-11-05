/*
  NAME
    Configure.hpp - Header file of the Configure class.

  DESCRIPTION
    Wrapper class for getting configuration.
*/

#ifndef __CONFIGURE_HPP__
#define __CONFIGURE_HPP__

#define S_DAEMON "daemon"
#define S_SLEEP_INTL "sleep_interval"
#define S_CONF_NAME "config"

#define S_CONF "conf"
#define S_DATA "data"
#define S_HOME "/opt/psmon"
#define S_LOG "logs"
#define S_WORK "work"

#define ENV_CONF "PSMON_CONF"
#define ENV_DATA "PSMON_DATA"
#define ENV_HOME "PSMON_HOME"
#define ENV_LOG "PSMON_LOG"
#define ENV_WORK "PSMON_WORK"

#define UNIX_DOMAIN ".unix.domain"

#include "ConfigFile.hpp"
#include <string>

class Configure
{
  public:
    static Configure* instance(const std::string& config_filename);
    static void destory();
    static std::string get_aes_key();
    static std::string get_conf_dir();
    static std::string get_data_dir();
    static std::string get_home_dir();
    static std::string get_log_dir();
    static std::string get_work_dir();
    bool configured();
    unsigned int get_sleep_interval();
    std::string get_conf_filename();
  protected:
    static Configure* _instance;
    Configure(const std::string& config_filename);
    ~Configure();
  private:
    static const std::string _aes_key;
    static std::string _conf_dir;
    static std::string _data_dir;
    static std::string _home_dir;
    static std::string _log_dir;
    static std::string _work_dir;
    ConfigFile* _config_file;
    unsigned int _sleep_interval;
    std::string _daemon_name;
    std::string _conf_filename;
};

#endif
