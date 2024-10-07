#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define BUFFER_SIZE 4096


int main()
{
	int serverSock = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSock == -1) {
		std::cerr << "error while using socket(2)\n";
		std::exit(1);
	}
	sockaddr_in x;
	std::memset(x.sin_zero, 0, 8 * sizeof(unsigned char));
	x.sin_port = htons(8080);
	x.sin_family = AF_INET;
	x.sin_addr.s_addr = INADDR_ANY;

	int s = bind(serverSock, (sockaddr*) &x, sizeof(x));
	if (s < 0) {
		std::cerr << "error while using bind(2)\n";
		std::exit(1);
	}
	
	if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		perror("setsockopt");
		exit(1);
	} 

	return 0;
}




























// int server()
// {

// 	int fdSocket = socket(AF_INET, SOCK_STREAM, 0);

// 	sockaddr_in address;
// 	address.sin_family = AF_INET;
// 	address.sin_port = htons(PORT);
// 	address.sin_addr.s_addr = htonl(INADDR_ANY);

// 	bind(fdSocket, (const sockaddr *)(&address), sizeof(address));

// 	listen(fdSocket, 10);

// 	bool active = true;
// 	int connection;
// 	while (active)
// 	{
// 		unsigned long resultLen = sizeof(sockaddr);
// 		std::cout << "Listening on Port: " << PORT << std::endl;
// 		connection = accept(fdSocket, (struct sockaddr *)(&address), (socklen_t *)&resultLen);

// 		char buffer[BUFFER_SIZE];
// 		ssize_t bytesRead = read(connection, buffer, BUFFER_SIZE);
// 		std::cout << "Le message fait: " << bytesRead << " characteres" << std::endl;
// 		std::cout << buffer << std::endl;

// 		std::string content = "<h1>Bonjour, je suis un serveur HTTP tout simple!</h1>";
// 		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
// 		send(connection, response.c_str(), response.size(), 0);
// 		close(connection);
// 	}

// 	close(fdSocket);

// 	return (EXIT_SUCCESS);
// }