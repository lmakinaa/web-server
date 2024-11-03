#include "includes/webserv.h"

int main(int ac, char *av[])
{

    signal(SIGPIPE, SIG_IGN);

    int confErr;
    WebServ    main;

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


    main.run();
    




    return (0);
}