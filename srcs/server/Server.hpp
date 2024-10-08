#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <climits>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class Server
{

public:
    Server(char* const ipAddr, int port);
    void init();
    void run();
    ~Server();


private:
    char* const m_ipAddr;
    unsigned short m_port;
    int m_serverSocket;


};

#endif