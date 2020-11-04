/*
  NAME
    Utils.hpp - Header file of the utilities.

  DESCRIPTION
    Some utilities.
*/

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

#define DELIMS " \t\r\n"

bool create_file(const std::string& filename);

bool file_exist(const std::string& filename);

bool path_exist(const std::string& pathname);

bool is_absolute_path(const std::string& filename);

bool last_day_of_month(const unsigned int the_date);

int days_diff(const unsigned int later_date, const unsigned int earlier_date);

unsigned int days_add(const unsigned int now, const int diff);

void match_filename(std::string& filename, const unsigned int date);

void* move_pointer(void* p, const long long offset);

std::string trim(const std::string& source);

#endif
