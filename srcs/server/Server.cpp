
#include "Server.hpp"


bool    Server::isAllowedDirective(std::string direc)
{
    std::string arr[7] = {"listen", "host", "server_name", "error_page", "client_max_body_size", "root", "index"};

    for (int i = 0; i < 7; i++)
    {
        if (arr[i] == direc)
            return (1);
    }
    return (0);
}



void    server()
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return ;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return ;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return ;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return ;
    }

    std::cout << "Server is running on port 8080" << std::endl;

    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            std::cerr << "Accept failed" << std::endl;
            return ;
        }
        std::cout << "Connection accepted" << std::endl;
        std::cout << "Recived http request: \n" << std::endl;
        char buf[1024] = {0};
        int read_bytes;
        read_bytes = read(client_fd, buf, 1024);

        if (read_bytes < 0)
        {
            std::cerr << "Failed to get the request" << std::endl;
            close(client_fd);
            continue;
        }

        buf[read_bytes] = '\0';
        std::cout << buf << std::endl;


        const char *str =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n" 
                    "Content-Length: 48\r\n"
                    "\r\n"
                    "<html><body><h1>Hello, World!</h1></body></html>";


        send(client_fd, str, strlen(str), 0);
        std::cout << "Response Sended" <<std::endl;
        close(client_fd);
    }

    close(server_fd);
}
