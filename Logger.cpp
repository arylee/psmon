/*
  NAME
    Logger.cpp - Body file of the Logger class.

  DESCRIPTION
    Simplify write log to file.
    日志类，如果不指定记录文件，则在stdout上输出
*/

#include "Logger.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

// 多线程的线程锁，防止多个线程同时写文件
pthread_mutex_t* Logger::mutex = NULL;
// 日志输出流
std::ofstream* Logger::out_stream = NULL;
// 日志文件的文件名
std::string Logger::_filename;
// 日志文件大小限制
const unsigned long Logger::_max_length = 10 * 1024 * 1024;

// 日志记录级别
const char* Logger::level_str[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

// 开始将日志记录到指定文件中
void Logger::start(const std::string& filename)
{
  _filename = filename;
  // 创建输出流，并打开指定文件
  out_stream = new std::ofstream();
  if(out_stream) {
    out_stream->open(filename.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::app);
  }
  // 创建线程锁，并初始化
  mutex = new pthread_mutex_t();
  pthread_mutex_init(mutex, NULL);
}

// 停止将日志记录到文件中，输出到stdout
void Logger::stop()
{
  // 关闭并释放输出流
  if(out_stream) {
    out_stream->close();
    delete out_stream;
    out_stream = NULL;
  }
  // 销毁线程锁
  if(mutex) {
    pthread_mutex_destroy(mutex);
    delete mutex;
    mutex = NULL;
  }
}

// 记录日志信息
void Logger::write_message(const logger_level level, const std::string& file,
                           const unsigned int line, const std::string& func,
                           const std::string& msg)
{
  time_t t = time(0);
  char ts[32];
  strftime(ts, sizeof(ts), "%F %T", localtime(&t));
  std::stringstream buffer;
  // 日志中的时间
  buffer <<  "[" << ts << "] ";
  // 日志级别
  buffer << "<" << level_str[level] << ">\t";
  // 日志内容
  buffer << msg;
  if(level <= Logger::log_level_debug) {
    buffer << "\n\t" << file << ":" << line;
  }
  if(Logger::log_level_trace == level) {
    buffer << " " << func << "()";
  }
  // 记录到文件
  if(mutex && out_stream && out_stream->is_open()) {
    // 锁定线程锁
    pthread_mutex_lock(mutex);
    // 向文件中写入日志内容
    *out_stream << buffer.str() << std::endl;
    unsigned long file_length = out_stream->tellp();
    std::stringstream ss;
    if(file_length > _max_length) {
      std::string backup_file;
      std::string original_file;
      backup_file.reserve(256);
      original_file.reserve(256);
      for(int i = 9; i > 0; ) {
        ss.str("");
        ss << "." << i--;
        backup_file = _filename + ss.str();
        ss.str("");
        ss << "." << i;
        original_file = _filename + ss.str();
        if(file_exist(original_file)) {
          rename(original_file.c_str(), backup_file.c_str());
        }
      }
      out_stream->close();
      rename(_filename.c_str(), backup_file.c_str());
      out_stream->open(_filename.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::app);
    }
    // 释放线程锁
    pthread_mutex_unlock(mutex);
  } else {
    // 记录到stdout
    std::cout << buffer.str() << std::endl;
  }
}
