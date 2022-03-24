/*
  NAME
    ProcessManager.hpp - Header file of the Process Manager class.

  DESCRIPTION
    Process Manager.
*/

#ifndef __PROCESS_MANAGER_HPP__
#define __PROCESS_MANAGER_HPP__

#include "Configure.hpp"
#include "WorkerProcess.hpp"

#define S_COMMAND "command"
#define S_WORK_DIR "work-dir"

#define S_OK "OK"
#define S_ERROR "ERROR"
#define S_SKIP "SKIP"
#define S_NO_WORKER "NO SUCH WORKER"

class ProcessManager {
  public:
    static ProcessManager* instance(std::string filename);
    static void destory();
    static ProcessManager* _instance;
    std::string start(std:: string name);
    std::string stop(std:: string name);
    std::string status();
    std::string reload();
    void start_worker_processes();
  protected:
    ProcessManager(std::string filename);
    ~ProcessManager();
  private:
    pthread_mutex_t* _mutex;
    std::string _filename;
    ConfigFile* _config_file;
    std::map<std::string, WorkerProcess> _processes;
    void relaunch(int old_pid);
};

#endif
