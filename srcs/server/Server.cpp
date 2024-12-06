
#include "Server.hpp"

s_eventData::~s_eventData() throw() {
	if (resData)
		delete resData;
	if (reqData)
		delete reqData;
}

Server::Server()
    : m_sockAddress ()
    , m_sockLen ()
    , m_sEventData ("server socket", (t_sockData*)NULL)
{
}

Server::~Server()
{
    close(m_socket);
}

void Server::init()
{
    if ((m_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error((std::string("socket(2): ") + strerror(errno)));

    int opt = 1;
    std::memset(m_sockAddress.sin_zero, 0, 8 * sizeof(unsigned char));
    m_sockAddress.sin_port = htons(stoi(serv[0].directives["listen"].values[0]));
    m_sockAddress.sin_family = AF_INET;

    m_sockLen = sizeof(m_sockAddress);

    if (serv[0].directives["host"].values[0] == "any")
        serv[0].directives["host"].values[0] = "0.0.0.0";
    else if (serv[0].directives["host"].values[0] == "localhost")
        serv[0].directives["host"].values[0] = "127.0.0.1";

    m_address = serv[0].directives["host"].values[0];
    m_port = serv[0].directives["listen"].values[0];

    struct addrinfo hints, *result;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(serv[0].directives["host"].values[0].c_str(), NULL, &hints, &result);
    if (status != 0) 
        throw std::runtime_error("getaddrinfo error: " + std::string(gai_strerror(status)));

    m_sockAddress.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
    freeaddrinfo(result);

    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error((std::string("setsockopt(2): ") + strerror(errno)));

    KQueue::setFdNonBlock(m_socket);

    if (bind(m_socket, (sockaddr*) &m_sockAddress, m_sockLen) == -1)
        throw std::runtime_error((std::string("bind(2): ") + strerror(errno)));

    if (listen(m_socket, SOMAXCONN) == -1)
        throw std::runtime_error((std::string("listen(2): ") + strerror(errno)));

    m_sockData.sockAddress = &m_sockAddress;
    m_sockData.sockLen = &m_sockLen;

    m_sEventData.serverData = &m_sockData;
    m_sEventData.s = &this->serv;
}