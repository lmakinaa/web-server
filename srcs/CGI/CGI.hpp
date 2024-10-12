#pragma once

#include <iostream>
#include <unistd.h>

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

#ifndef BUFF_SIZE
# define BUFF_SIZE 64000
#endif

class CGI
{

public:
    static void scriptToHtml(const char* cgiPath, const char* argv[], std::string& buff);



};