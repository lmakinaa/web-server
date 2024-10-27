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

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

typedef struct s_eventData {
    const char* type;
    void* data;
    s_eventData(const char* type, void* data): type(type), data(data){}
} t_eventData;

class KQueue
{

public:
    static int createKq();
    static void closeKq();
    static int getFd() {return m_fd;}
    static int watchFd(int fd, t_eventData* evData);
    static void removeFd(int fd);
    static int getEvents(struct kevent* buffArray, int size);
    static void setFdNonBlock(int fd);
    
    static struct kevent m_keventBuff;

private:
    static int m_fd;

};