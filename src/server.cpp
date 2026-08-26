#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);

	if (serverFd == -1) {
		std::cout << "socker failed" << "\n";
		return 1;
	}

	std::cout << "got fd: " << serverFd << "\n"; 

	sockaddr_in addr{}; 
	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = INADDR_ANY; 
	addr.sin_port = htons(9090); 

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

	sockaddr_in clientAddr{}; 
	socklen_t clientLen = sizeof(clientAddr); 
	int clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientLen);

	if (clientFd == -1){ 
		std::cout << "Accept failed \n"; 
		return 1;
	}

	std::cout << "Client connected! fd = " << clientFd << "\n";

	close(clientFd); 
	close(serverFd);


		


	return 0;

}
