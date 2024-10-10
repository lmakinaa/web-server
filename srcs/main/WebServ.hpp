
#pragma once


#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#include "../server/Server.hpp"

class   WebServ
{

public:
    void run();

public:
    std::vector<Server> servers;

};