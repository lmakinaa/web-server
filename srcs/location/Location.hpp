#pragma once

#include <iostream>
#include <map>
#include "../directive/Directive.hpp"

class   Location
{
    public:
        std::map<std::string, Directive> directives;


    bool    isAllowedDirective(std::string direc);
};