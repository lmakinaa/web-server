#pragma once

#include <iostream>
#include <unistd.h>
#include "../KQueue/KQueue.hpp"

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

#ifndef BUFF_SIZE
# define BUFF_SIZE 64000
#endif

#define NOCHILD -2

typedef enum request_method {
    POST,
    GET,
    DELETE
} t_method;

class CGI
{

public:
    static void runScript(t_method reqMethod, const char* cgiPath, const char* argv[], std::string& postData, long fd);
    static void readOutput(int fd, std::string& buff);

    static t_eventData m_cgiEventData;


};