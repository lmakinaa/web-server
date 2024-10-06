
#include "Location.hpp"



bool    Location::isAllowedDirective(std::string direc)
{
    std::string arr[7] = {"autoindex", "allow_methods", "return", "alias", "cgi_path", "root", "index"};

    for (int i = 0; i < 7; i++)
    {
        if (arr[i] == direc)
            return (1);
    }
    return (0);
}