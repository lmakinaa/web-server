
#include "Server.hpp"
#include <fstream>
#include <vector>
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
            HttpRequest req;
           
            req.ReadRequest(client_fd);
            HttpResponse res = HttpResponse();
    
            // std::ifstream file ();
            std::ifstream file(req.GetUri().substr(1), std::ios::binary);
            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            file.read(&buffer[0], size);
    
            std::cout << "\033[1;31m"<< buffer.size() <<"\033[0m\n";
            res.SetBody(buffer);
            res.SetResponseCode("200 OK");
            res.SetContentType(WhatContentType(req.GetUri().substr(1)));

            // if (file.is_open())
            // {
            //     std::string line;
            //     while (getline(file, line))
            //         res->SetResponse(line);

            //     res->SetResponseCode("200 OK");
            //     res->SetContentType(WhatContentType(req.GetUri().substr(1)));
            //     res->SetConnection("close");
            //     file.close();
            // }
            // else
            // {
            //     puts("<---------------------Response404---------------------->");
            //     res->SetResponseCode("404 Not Found");
            //     res->SetContentType("text/html");
            //     res->SetResponse("<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr></body></html>");
            // }
            std::vector<char> str = res.BuildResponse();
            send(client_fd,&str[0], str.size(), 0);
            // std::cout << "<---------------------Response200---------------------->" << std::endl;
            // std::cout << res->GetResponse() << std::endl;
            // delete res;
        }
        catch(std::exception& e){
            send(client_fd, e.what(), strlen(e.what()), 0);
            puts("<---------------------Response400---------------------->");
            puts(e.what());

        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
