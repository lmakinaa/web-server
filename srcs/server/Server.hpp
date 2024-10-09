#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <climits>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// For kqueue
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

class Server
{

public:
    Server();
    void init();
    ~Server();
    int getSocket() const {return m_socket;}

    sockaddr_in m_sockAddress;
    socklen_t m_sockLen;


private:
    int m_socket;
    // std::map<std::string, Directive> directives;
    // std::map<std::string, Location> locations;


};

#endif