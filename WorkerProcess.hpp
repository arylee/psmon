/*
  NAME
    WorkerProcess.hpp - Header file of the Worker Process class.

  DESCRIPTION
    Worker Process.
*/

#ifndef __WORKER_PROCESS_HPP__
#define __WORKER_PROCESS_HPP__

#include <string>
#include <vector>

class WorkerProcess {
  public:
    WorkerProcess();
    WorkerProcess(std::string name, std::string command, std::string work_dir, bool auto_start);
    ~WorkerProcess();
    bool launch();
    void relaunch(int old_pid);
    pid_t get_pid();
    void set_pid(pid_t pid);
  private:
    int _length;
    pid_t _pid;
    std::string _name;
    std::string _command;
    std::string _work_dir;
    std::vector<std::string> _tokens;
};

#endif
