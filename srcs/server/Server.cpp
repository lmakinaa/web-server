
#include "Server.hpp"

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    HttpRequest req;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is running on port 8080" << std::endl;

    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            std::cerr << "Accept failed" << std::endl;
            return -1;
        }
        std::cout << "Connection accepted" << std::endl;
        std::cout << "Recived http request: \n" << std::endl;
        char buf[1024] = {0};
        int read_bytes;

        try{
            read_bytes = read(client_fd, buf, 1024);
            req.ParseRequest(buf);
            puts("here2");

             const char *str =
                    "HTTP/1.1 200 Not Found\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n" 
                    "\r\n"
                    "<html><body><h1>Hello, World!</h1></body></html>";
                    send(client_fd,str, strlen(str), 0);
        }
        catch(std::exception& e){
            send(client_fd, e.what(), strlen(e.what()), 0);
            puts("here1");
        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
