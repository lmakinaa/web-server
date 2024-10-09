#include "Server.hpp"

// static int getKQueue()
// {
// 	static int _v = -2;

// 	if (_v == -2) {
// 		if ((_v = kqueue()) == -1)
// 			throw std::runtime_error((std::string("kqueue(2): ") + strerror(errno)));
// 	}
// 	return _v;
// }

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
	m_sockAddress.sin_port = htons(8080);
	m_sockAddress.sin_family = AF_INET;

	m_sockLen = sizeof(m_sockAddress);

	// if (inet_pton(AF_INET, directives["host"].values[0].c_str(), &m_sockAddress.sin_addr.s_addr) == 0) 
	if (inet_pton(AF_INET, "127.0.0.1", &m_sockAddress.sin_addr.s_addr) == 0) 
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
