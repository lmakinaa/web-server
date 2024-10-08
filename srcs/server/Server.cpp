#include "Server.hpp"

Server::Server(char* const ipAddr, int port)
	: m_ipAddr (ipAddr)
	, m_port (port)
{
	if (port < 1024 || port > USHRT_MAX)
		throw std::runtime_error("invalide port number");
}

void Server::init()
{
	if ((m_serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error((std::string("socket(2)") + strerror(errno)));
	sockaddr_in address;
	
}

void Server::run()
{

}

Server::~Server()
{
}
