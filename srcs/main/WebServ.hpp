
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

#ifndef M_DEBUG
# define M_DEBUG 0
#endif

class   WebServ
{

public:
    void run();
    int handleNewConnection(Server& s, struct kevent* current);
    int handleOldConnection(struct kevent* current);

public:
    std::vector<Server> servers;

private:
    int m_openedSockets;

};