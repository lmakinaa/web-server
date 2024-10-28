#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <sstream>

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

class HttpResponse{
    private :
        std::string Version;
        std::string ResponseCode;
        std::string ContentType;
        std::string Connection;
        std::vector<char> Body;
        int clientSocket;
        int responseFd;

    public :
        bool ended;
        HttpResponse(int clientSocket, int fd) : Version("HTTP/1.1"), ResponseCode("200 OK"), ContentType("text/html"), Connection("close"), clientSocket(clientSocket), responseFd(fd), ended(false) {
            std::cerr << "tconstructa\n";
            send(clientSocket, "HTTP/1.1 200\r\nContent-Type: image/jpg\r\nConnection: keep-alive\r\nTransfer-Encoding: chunked\r\n\r\n", 93, 0);
        }
        ~HttpResponse() {close(responseFd);}
        void sendingResponse();
        void SetVersion(std::string value);
        void SetResponseCode(std::string value);
        void SetContentType(std::string value);
        void SetConnection(std::string value);
        void SetBody(std::vector<char> Body);

        std::string GetVersion();
        std::string GetResponseCode();
        std::string GetContentType();
        std::string GetConnection();
        const std::vector<char>& GetBody();
        const std::vector<char> BuildResponse();

};

std::string WhatContentType(std::string uri);

