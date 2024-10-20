#include "includes/webserv.h"


void    affiche(Main &main)
{
    for (size_t i = 0; i < main.servers.size(); i++)
    {
        std::map<std::string, Directive>::iterator it = main.servers[i].directives.begin();
        for ( ; it != main.servers[i].directives.end(); it++)
        {
            std::cout << (*it).first << ": ";
            for (size_t j = 0; j < (*it).second.values.size(); j++)
            {
                std::cout << (*it).second.values[j] << " ";
            }
            std::cout << std::endl;
        }

        std::map<std::string, Location>::iterator it2 = main.servers[i].locations.begin();
        for ( ; it2 != main.servers[i].locations.end(); it2++)
        {
            std::cout << "location " << (*it2).first << " {\n";


            std::map<std::string, Directive>::iterator it3 = (*it2).second.directives.begin();
            for ( ; it3 != (*it2).second.directives.end(); it3++)
            {
                std::cout << (*it3).first << ": ";
                for (size_t j = 0; j < (*it3).second.values.size(); j++)
                {
                    std::cout << (*it3).second.values[j] << " ";
                }
                std::cout << std::endl;
            }

            std::cout << "}\n";
        }
    }
}

int main(int ac, char *av[])
{
    int confErr;
    Main    main;

    if (ac != 2)
    {
        std::cerr << "Invalid Args Number" << std::endl;
        return (EXIT_FAILURE);

    }
    confErr = parseConfigFile(static_cast<std::string>(av[1]), main);
    if (confErr == 0)
    {
        std::cerr << "Invalid config file" <<std::endl;
        return (EXIT_FAILURE);
    }

    if (confErr == -1)
        return (EXIT_FAILURE);
    // server();

    _GET_DELETE(main);


    // affiche(main);
    return (0);
}