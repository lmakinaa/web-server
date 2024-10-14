
#pragma once


#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#include "../KQueue/KQueue.hpp"
#include "../server/Server.hpp"
#include "../CGI/CGI.hpp"

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

class   WebServ
{

public:
    WebServ();
    void run();
    int handleNewConnection(struct kevent* current);
    int handleExistedConnection(struct kevent* current);
    void sendResponse(struct kevent* current);

public:
    std::vector<Server> servers;
    t_eventData m_cEventData;

private:
    int m_openedSockets;

};