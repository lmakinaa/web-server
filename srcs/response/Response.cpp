
#include "Response.hpp"


int checkValidLocation(std::map<std::string, Location>::iterator it, std::string requestPath)
{



    return (1);
}


void    Response(Main &main)
{
    Server serv = main.servers[0];
    std::string requestPath = "/index.html";

    std::map<std::string, Location>::iterator it = serv.locations.begin();
    

    /* ======= Looking for the request path in server locations ======= */
    for ( ; it != serv.locations.end(); it++)
    {
        if ((*it).first == requestPath)
            break ;
    }

    /* ======= Found the matching path ======= */
    if (it != serv.locations.end())
    {
        /* ======= check the existance of the index ======= */
        if (checkValidLocation(it, requestPath) == -1)
        {
            if (isDirectory(requestPath) &&  (*it).second.directives["autoindex"].values[0] == "on")
            {
                // listAllCurrentDirectories();
            }
            else
            {
                // Response with 403 forbidden
            }
        }
        else
        {
            // Response with the index found
        }
    }
    else
    {
        // Search in server root
    }


    
}