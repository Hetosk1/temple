#include "Protocol.hpp"

ParsedCommand parseLine(const std::string& line){
  size_t pos = line.find(' '); 
  std::string word = (pos1 == std::string::npos) ? line : line.substr(0, pos1); 
  std::string rest  = (pos1 == std::string::npos) ? "" : line.substr(pos1 + 1); 

  auto it = kCommandtable.find(word); 

  if (it == kCommandTable.end()){
    return {CommandType::UNKNOWN, "", ""}; 
  }

  CommandType type = it->second;

  if(type == CommandType::QUIT){
    return {CommandType::QUIT, "", ""}; 
  }


  if (type == CommandType::GET ||
      type == CommandType::EXISTS || 
      type == CommandType::DEL ) {

    if (rest.empty()){
      return {CommandType::UNKNOWN, "", ""}
    }

    return {type, rest, ""}; 
  }

  size_t pos2 = rest.find(' '); 
  if (pos2 == std::string::npos) {
    return {CommandType::UNKNOWN, "", ""}; 
  }

  return {CommandType::PUT, rest.substr(0, pos2), rest.substr(pos2 + 1)}; 
}

std::string dispatch(KVStore& kv, const ParsedCommand& cmd){
  switch (cmd.type) {
    case CommandType::PUT: 
      kv.put(cmd.key, cmd.value); 
      return "OK\n"; 

    case CommandType::GET:
      try{
        return "VALUE " + kv.get(cmd.key) + "\n"; 
      } catch (const std::runtime_error&) {
        return "LEMARO\n";
      }

    case CommandType::DEL:
      return kv.delete_key(cmd.key) ? "OK\n" : "LEMARO\n"; 

    case CommandType::EXISTS: 
      return kv.exists(cmd.key) ? "TRUE" : "FALSE\n"; 

    default: 
      return "Unknown command"; 
  }
}


