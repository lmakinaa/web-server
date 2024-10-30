#pragma once

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdexcept>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include "../server/Server.hpp"

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

class KQueue
{

public:
    static int createKq();
    static void closeKq();
    static int getFd() {return m_fd;}
    static int watchState(int fd, t_eventData* evData, int type);
    static void removeWatch(int fd, int type);
    static int getEvents(struct kevent* buffArray, int size);
    static void setFdNonBlock(int fd);
    
    static struct kevent m_keventBuff;

private:
    static int m_fd;

};