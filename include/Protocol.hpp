#pragma once 

#include <string> 
#include "KVStore.hpp"

enum class CommandType { PUT, GET, DEL, EXISTS, QUIT, UNKNOWN }; 

struct ParsedCommand {
  CommandType type; 
  std::string key; 
  std::string value; 
};

ParsedCommand parseLine(const std::string& line); 

std::string dispatch(KVStore& kv, const ParsedCommand cmd); 

inline const std::unordered_map<std::string, CommandType> kCommandTable = {
  {"PUT",    CommandType::PUT},
  {"GET",    CommandType::GET}, 
  {"DEL",    CommandType::DEL}, 
  {"EXISTS", CommandType::EXISTS}, 
  {"QUIT",   CommandType::QUIT}
};




