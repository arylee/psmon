/*
  NAME
    PsmonDaemon.hpp - Header file of the PSMON daemon class.

  DESCRIPTION
    Processing monitor daemon program.
*/

#ifndef __PSMON_DAEMON_HPP__
#define __PSMON_DAEMON_HPP__

#include "BaseDaemon.hpp"
#include "Logger.hpp"
#include "ProcessManager.hpp"
#include <string>
#include <semaphore.h>

#define S_CMD_STATUS "cmd:status"
#define S_CMD_START "cmd:start"
#define S_CMD_STOP "cmd:stop"

#define S_SYNTAX_ERROR "SYNTAX ERROR"

class PsmonDaemon : public BaseDaemon
{
  public:
    static PsmonDaemon* instance(const std::string& daemon_name);
    static void destory();
    static void version();
    PsmonDaemon(const std::string& daemon_name);
    virtual ~PsmonDaemon();
    virtual bool init();
    virtual bool prepare();
    virtual void process();
  protected:
    static PsmonDaemon* _instance;
  private:
    static sem_t* _semaphore;
    ProcessManager* _ps_man;
};

#endif
