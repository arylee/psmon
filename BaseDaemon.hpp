/*
  NAME
    BaseDaemon.hpp - Header file of the base daemon class.

  DESCRIPTION
    Simplify daemon program.
*/

#ifndef __BASE_DAEMON_HPP__
#define __BASE_DAEMON_HPP__

#include "Configure.hpp"
#include <string>

class BaseDaemon
{
  public:
    static bool running;
    void run();
  protected:
    static void skeleton_daemon();
    unsigned int _sleep_interval;
    Configure* _configure;
    std::string _daemon_name;
    BaseDaemon();
    ~BaseDaemon();
    virtual bool init() = 0;
    virtual bool prepare() = 0;
    virtual void process() = 0;
};

#endif
