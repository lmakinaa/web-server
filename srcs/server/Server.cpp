
#include "Server.hpp"

Server::Server()
{
}

Server::~Server()
{
}

void Server::init()
{

	if ((m_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error((std::string("socket(2): ") + strerror(errno)));

	int opt = 1;
	std::memset(m_sockAddress.sin_zero, 0, 8 * sizeof(unsigned char));
	// m_sockAddress.sin_port = htons(std::stoi(directives["listen"].values[0]));
	m_sockAddress.sin_port = htons(stoi(directives["listen"].values[0]));
	m_sockAddress.sin_family = AF_INET;

	m_sockLen = sizeof(m_sockAddress);

	// if (inet_pton(AF_INET, directives["host"].values[0].c_str(), &m_sockAddress.sin_addr.s_addr) == 0) 
	if (inet_pton(AF_INET, directives["host"].values[0].c_str(), &m_sockAddress.sin_addr.s_addr) == 0) 
		throw std::runtime_error("invalide Ip address");


	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error((std::string("setsockopt(2): ") + strerror(errno)));

	int f = fcntl(m_socket, F_GETFL);
	fcntl(m_socket, F_SETFL, f | O_NONBLOCK);

	if (bind(m_socket, (sockaddr*) &m_sockAddress, m_sockLen) == -1)
		throw std::runtime_error((std::string("bind(2): ") + strerror(errno)));

	if (listen(m_socket, SOMAXCONN) == -1)
		throw std::runtime_error((std::string("listen(2): ") + strerror(errno)));

}






// void    server()
// {
//     int server_fd;
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);

//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         std::cerr << "Socket failed" << std::endl;
//         return ;
//     }

//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
//         std::cerr << "setsockopt failed" << std::endl;
//         return ;
//     }

//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(8080);

//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         std::cerr << "Bind failed" << std::endl;
//         return ;
//     }

//     if (listen(server_fd, 3) < 0) {
//         std::cerr << "Listen failed" << std::endl;
//         return ;
//     }

//     std::cout << "Server is running on port 8080" << std::endl;

//     while (true) {
//         int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
//         if (client_fd < 0) {
//             std::cerr << "Accept failed" << std::endl;
//             return ;
//         }
//         std::cout << "Connection accepted" << std::endl;
//         std::cout << "Recived http request: \n" << std::endl;
//         char buf[1024] = {0};
//         int read_bytes;
//         read_bytes = read(client_fd, buf, 1024);

//         if (read_bytes < 0)
//         {
//             std::cerr << "Failed to get the request" << std::endl;
//             close(client_fd);
//             continue;
//         }

//         buf[read_bytes] = '\0';
//         std::cout << buf << std::endl;


//         const char *str =
//                     "HTTP/1.1 200 OK\r\n"
//                     "Content-Type: text/html\r\n"
//                     "Connection: close\r\n" 
//                     "Content-Length: 48\r\n"
//                     "\r\n"
//                     "<html><body><h1>Hello, World!</h1></body></html>";


//         send(client_fd, str, strlen(str), 0);
//         std::cout << "Response Sended" <<std::endl;
//         close(client_fd);
//     }

//     close(server_fd);
// }
