/*
  NAME
    Logger.hpp - Header file of the Logger class.

  DESCRIPTION
    Simplify write log to file.
*/

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <fstream>
#include <sstream>
#include <string>
#include <pthread.h>

class Logger
{
  public:
    enum logger_level {
      min_level,
      log_level_trace = min_level,
      log_level_debug,
      log_level_info,
      log_level_warn,
      log_level_error,
      log_level_fatal,
      max_level = log_level_fatal
    };
  public:
    static void start(const std::string& filename);
    static void stop();
    static void write_message(const logger_level level, const std::string& file,
                              const unsigned int line, const std::string& func,
                              const std::string& msg);
  protected:
    static pthread_mutex_t* mutex;
    static std::ofstream* out_stream;
  private:
    static const char* level_str[];
    static const unsigned long _max_length;
    static std::string _filename;
};

#ifndef CURRENT_LOGGER_LEVEL
#define CURRENT_LOGGER_LEVEL 0
#endif

#define LOG_TRACE_MSG(msg) if(Logger::log_level_trace >= CURRENT_LOGGER_LEVEL) \
    Logger::write_message(Logger::log_level_trace, __FILE__, __LINE__, \
                          __FUNCTION__, msg)

#define LOG_DEBUG_MSG(msg) if(Logger::log_level_debug >= CURRENT_LOGGER_LEVEL) \
    Logger::write_message(Logger::log_level_debug, __FILE__, __LINE__, \
                          __FUNCTION__, msg)

#define LOG_INFO_MSG(msg)  if(Logger::log_level_info >= CURRENT_LOGGER_LEVEL) \
    Logger::write_message(Logger::log_level_info, __FILE__, __LINE__, \
                          __FUNCTION__, msg)

#define LOG_WARN_MSG(msg)  if(Logger::log_level_warn >= CURRENT_LOGGER_LEVEL) \
    Logger::write_message(Logger::log_level_warn, __FILE__, __LINE__, \
                          __FUNCTION__, msg)

#define LOG_ERROR_MSG(msg) if(Logger::log_level_error >= CURRENT_LOGGER_LEVEL) \
    Logger::write_message(Logger::log_level_error, __FILE__, __LINE__, \
                          __FUNCTION__, msg)

#define LOG_FATAL_MSG(msg) if(Logger::log_level_fatal >= CURRENT_LOGGER_LEVEL) \
    Logger::write_message(Logger::log_level_fatal, __FILE__, __LINE__, \
                          __FUNCTION__, msg)

#endif
