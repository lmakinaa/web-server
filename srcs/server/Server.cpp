
#include "Server.hpp"
#include <fstream>

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

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

    while (true) 
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            std::cerr << "Accept failed" << std::endl;
            return -1;
        }
        int read_bytes;

        try{
            char buf[1024] = {0};
            HttpRequest req;
            read_bytes = read(client_fd, buf, 1024);
            req.ParseRequest(buf);
            HttpResponse *res = new HttpResponse();

            res->SetVersion("HTTP/1.1");
            res->SetContentType("text/html");
            res->SetConnection("close");
            
    
            std::ifstream file (req.GetUri().substr(1));
            if (file.is_open())
            {
                std::string line;
                while (getline(file, line))
                    res->SetResponse("\n" + line );
                res->SetResponseCode("200 OK");
                file.close();
            }
            else
            {
                res->SetResponseCode("404 Not Found");
                res->SetResponse("<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr></body></html>");
            }
            std::string str = res->BuildResponse();
            send(client_fd,str.c_str(), strlen(str.c_str()), 0);
            delete res;

        }
        catch(std::exception& e){
            send(client_fd, e.what(), strlen(e.what()), 0);
            puts(e.what());
        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
