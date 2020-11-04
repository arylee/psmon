/*
  NAME
    Utils.hpp - Body file of the utilities.

  DESCRIPTION
    Some utilities.
    常用函数
*/

#include "Utils.hpp"
#include "Logger.hpp"
#include "Symbol.hpp"
#include <ctime>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

// 创建文件
bool create_file(const std::string& filename)
{
  std::ofstream fs;
  fs.open(filename.c_str(), std::ios_base::binary | std::ios_base::out);
  bool result = fs.is_open();
  fs.close();
  return result;
}

// 判断文件是否存在
bool file_exist(const std::string& filename)
{
  struct stat buf;
  return (0 == stat(filename.c_str(), &buf));
}

// 判断目录是否存在
bool path_exist(const std::string& pathname)
{
  bool result = false;
  DIR* dirptr = opendir(pathname.c_str());
  if(NULL != dirptr) {
    result = true;
    closedir(dirptr);
  }
  return result;
}

// 判断是否使用了绝对路径
bool is_absolute_path(const std::string& filename)
{
  bool result = false;
  if("" != filename) {
    try {
      result = (C_SLASH == filename[0] || (C_DOT == filename[0] &&
                                           C_DOT == filename[1]));
    } catch(...) {
    }
  }
  return result;
}

// 判断是否是该月的最后一天
bool last_day_of_month(const unsigned int the_date)
{
  time_t t = time(0);
  tm* _tm = localtime(&t);

  _tm->tm_year = the_date / 10000 - 1900;
  int old_month = the_date % 10000;
  _tm->tm_mday = old_month % 100;
  _tm->tm_mon = (old_month / 100 - 1) % 12;
  old_month = _tm->tm_mon;

  t = mktime(_tm) + 24 * 60 * 60;
  _tm = localtime(&t);

  return (old_month != _tm->tm_mon);
}

// 获取二个日期之间相差的天数
int days_diff(const unsigned int later_date, const unsigned int earlier_date)
{
  time_t t = time(0);
  tm* tm_later = localtime(&t);
  tm_later->tm_year = later_date / 10000 - 1900;
  int month = later_date % 10000;
  tm_later->tm_mday = month % 100;
  tm_later->tm_mon = (month / 100 - 1) % 12;
  time_t t_later = mktime(tm_later);
  tm* tm_earlier = localtime(&t);
  tm_earlier->tm_year = earlier_date / 10000 - 1900;
  month = earlier_date % 10000;
  tm_earlier->tm_mday = month % 100;
  tm_earlier->tm_mon = (month / 100 - 1) % 12;
  time_t t_earlier = mktime(tm_earlier);
  return (t_later - t_earlier) / (24 * 60 * 60);
}

// 获取指定日期加上若干天后的日期
unsigned int days_add(const unsigned int now, const int diff)
{
  unsigned int calc_date = 0;
  time_t t = time(0);
  tm* _tm = localtime(&t);
  if(_tm) {
    _tm->tm_year = now / 10000 - 1900;
    _tm->tm_mon = now % 10000;
    _tm->tm_mday = _tm->tm_mon % 100;
    _tm->tm_mon = (_tm->tm_mon / 100 - 1) % 12;

    t = mktime(_tm) + diff * 24 * 60 * 60;
    _tm = localtime(&t);
    calc_date = (_tm->tm_year + 1900) * 10000 +
      (_tm->tm_mon + 1) * 100 + _tm->tm_mday;
  }
  return calc_date;
}

// 替换文件名中的年、月、日
void match_filename(std::string& filename, const unsigned int date)
{
  std::string s_date;
  std::stringstream ss;
  std::string::size_type index;
  bool should_change = false;
  ss << date;
  s_date = ss.str();
  LOG_TRACE_MSG("The original filename is:[" + filename + "]");
  if(std::string::npos != filename.find("${")) {
    unsigned int length = filename.size();
    for(index = 0; index < length; index++) {
      switch(filename[index]) {
        case C_R_BRACE:
          if(should_change) {
            filename.erase(index, 1);
            length = filename.size();
            should_change = false;
          }
        case C_DOLLAR:
          if(index < length - 1 && C_L_BRACE == filename[index + 1]) {
            should_change = true;
            filename.erase(index, 2);
            length = filename.size();
          }
        case C_YEAR:
          if(should_change && index < length - 3 &&
             C_YEAR == filename[index + 1] && C_YEAR == filename[index + 2] &&
             C_YEAR == filename[index + 3]) {
            filename[index++] = s_date[0];
            filename[index++] = s_date[1];
            filename[index++] = s_date[2];
            filename[index] = s_date[3];
          }
          break;
        case C_MONTH:
          if(should_change && index < length - 1 &&
             C_MONTH == filename[index + 1]) {
            filename[index++] = s_date[4];
            filename[index] = s_date[5];
          }
          break;
        case C_DAY:
          if(should_change && index < length - 1 &&
             C_DAY == filename[index + 1]) {
            filename[index++] = s_date[6];
            filename[index] = s_date[7];
          }
          break;
      }
    }
  }
  if(should_change) {
    LOG_WARN_MSG("The filename format error, need more \"}\".");
  }
  LOG_TRACE_MSG("The changed filename is:[" + filename + "]");
}

// 移动指针
void* move_pointer(void* p, const long long offset)
{
  if(p) {
    long long q = offset + (long long) p;
    return (void*) q;
  }
  return p;
}

// 截断字符串中的左右无效字符
std::string trim(const std::string& source)
{
  std::string result(source);
  std::string::size_type index = result.find_last_not_of(DELIMS);
  if(std::string::npos != index) {
    result.erase(++index);
  }

  index = result.find_first_not_of(DELIMS);
  if(std::string::npos != index) {
    result.erase(0, index);
  } else {
    result.erase();
  }

  return result;
}
