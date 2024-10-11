#pragma once

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdexcept>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#ifndef M_DEBUG
# define M_DEBUG 0
#endif

class KQueue
{

public:
    static int createKq();
    static void closeKq();
    static int getFd() {return m_fd;}
    static int watchSocket(int fd);
    static void removeSocket(int fd);
    static int getEvents(struct kevent* buffArray, int size);

private:
    static int m_fd;
    static struct kevent m_keventBuff;

};