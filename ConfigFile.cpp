/*
  NAME
    ConfigFile.cpp - Body file of the ConfigFile class.

  DESCRIPTION
    Simplify read content from configuration file.
    读取配置文件信息
*/

#include "ConfigFile.hpp"
#include "Symbol.hpp"
#include "Utils.hpp"
#include <fstream>

// 构造函数
ConfigFile::ConfigFile(const std::string& filename)
{
  std::ifstream file(filename.c_str());
  std::string line;
  std::string name;
  std::string value;
  std::string section;
  std::string::size_type pos_equal;
  line.reserve(LINE_SIZE);
  while(getline(file, line)) {
    // 空行
    if(!line.size()) {
      continue;
    }
    // 注释行
    if(C_POUND == line[0] || C_SEMICOLON == line[0]) {
      continue;
    }
    // 块区
    if(C_L_BRACKET == line[0]) {
      std::string::size_type pos = line.find(C_R_BRACKET);
      if(std::string::npos != pos) {
        section = trim(line.substr(1, pos - 1));
        _sections.push_back(section);
        _content[section] = "";
      }
      continue;
    }
    // 配置内容行
    pos_equal = line.find(C_EQUAL);
    if(std::string::npos != pos_equal) {
      name  = trim(line.substr(0, pos_equal));
      value = trim(line.substr(pos_equal + 1));
      _content[section + C_SLASH + name] = value;
    }
  }
  file.close();
}

// 获取配置信息
const std::string& ConfigFile::value(const std::string& section, const std::string& entry)
{
  std::map<std::string, std::string>::const_iterator ci =  _content.find(section + C_SLASH + entry);
  if(_content.end() == ci) {
    throw "Section or entry does not exist.";
  }
  return ci->second;
}

// 判断指定的区块是否存在
bool ConfigFile::has_section(const std::string& section)
{
  std::map<std::string, std::string>::const_iterator ci = _content.find(section);
  if(_content.end() == ci) {
    return false;
  }
  return true;
}

// 获取所有的块列表
std::list<std::string>* ConfigFile::get_sections()
{
  return &_sections;
}

// 构造函数
ConfigFile::ConfigFile()
{
}
