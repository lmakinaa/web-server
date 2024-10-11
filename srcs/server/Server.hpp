
#pragma once

#include <iostream>
#include <climits>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>
#include <unistd.h>

#include "../configFile/Directive.hpp"
#include "../configFile/Location.hpp"


class   Server
{

public:
    Server();
    ~Server();
    void init();
    int getSocket() const {return m_socket;}

    sockaddr_in m_sockAddress;
    socklen_t m_sockLen;

    std::map<std::string, Directive> directives;
    std::map<std::string, Location> locations;

private:
    int m_socket;

};



/*          Server        */
void    server();