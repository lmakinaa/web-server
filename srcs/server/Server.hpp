
#pragma once

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>
#include <vector>
#include "../directive/Directive.hpp"
#include "../location/Location.hpp"


class   Server
{
    public:
        std::map<std::string, Directive> directives;
        std::map<std::string, Location> locations;

        bool    isAllowedDirective(std::string direc);
};



/*          Server        */
void    server();