/*
  NAME
    ConfigFile.hpp - Header file of the ConfigFile class.

  DESCRIPTION
    Simplify read content from configuration file.
*/

#ifndef __CONFIG_FILE_HPP__
#define __CONFIG_FILE_HPP__

#define LINE_SIZE 1024

#include <list>
#include <map>
#include <string>

class ConfigFile
{
  public:
    ConfigFile(const std::string& filename);
    const std::string& value(const std::string& section, const std::string& entry);
    bool has_section(const std::string& section);
    std::list<std::string>* get_sections();
  protected:
    std::map<std::string, std::string> _content;
    std::list<std::string> _sections;
    ConfigFile();
};

#endif
