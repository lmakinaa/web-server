
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

typedef struct s_eventData t_eventData;
class   Server;

#include "../KQueue/KQueue.hpp"

typedef struct s_eventData {
    const char* type;
    void* data;
    Server* s;
    s_eventData(const char* type, void* data): type(type), data(data), s() {}
} t_eventData;

typedef struct s_sockData {
    sockaddr_in* sockAddress;
    socklen_t* sockLen;
    s_sockData(): sockAddress (NULL), sockLen(NULL) {}
} t_sockData;

class   Server
{

public:
    Server();
    ~Server();
    void init();
    int getSocket() const {return m_socket;}

    sockaddr_in m_sockAddress;
    socklen_t m_sockLen;
    t_sockData m_sockData;
    struct s_eventData m_sEventData;

    std::map<std::string, Directive> directives;
    std::map<std::string, Location> locations;
    std::map<std::string, std::time_t> session_ids;

private:
    int m_socket;

};
