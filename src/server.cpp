#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Protocol.hpp"

int main() {
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);

	if (serverFd == -1) {
		std::cout << "socket failed" << "\n";
		return 1;
	}

	std::cout << "got fd: " << serverFd << "\n";

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(9090);

  int opt = 1;
  setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if(bind(serverFd, (sockaddr*)&addr, sizeof(addr)) == -1) {
    std::cout << "Bind failed \n";
    return 1;
  }

  std::cout << "Bound to port 9090 \n";

  if (listen(serverFd, 16) == -1) {
    std::cout << "listen failed \n";
    return 1;
  }

  std::cout << "Listening on port 9090 \n";

  KVStore kv;

  while(true){
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr); 
    int clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientLen);

    if (clientFd == -1){ 
      std::cout << "Accept failed \n"; 
      return 1;
    }

    std::cout << "Client connected! fd = " << clientFd << "\n";

    char buffer[512];
    std::string lineBuf;

    while(true){
      ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer), 0); 
      
      if (bytesRead <= 0) {
        break;
      }

      lineBuf.append(buffer, bytesRead);

      size_t pos = lineBuf.find('\n'); 
      if (pos == std::string::npos) {
        continue;
      }
      
      std::string line = lineBuf.substr(0, pos);
      lineBuf.clear();

      ParsedCommand cmd = parseLine(line);

      if (cmd.type == CommandType::QUIT) {
        break;
      }

      std::string response = dispatch(kv, cmd);
      send(clientFd, response.c_str(), response.size(), 0); 
    }

    std::cout << "Client disconnected" << std::endl;

    close(clientFd);

  }

  return 0;

}
