#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include "Request.hpp"

std::string WhatContentType(std::string uri);

#ifndef M_DEBUG
# define M_DEBUG 1
#endif

class HttpResponse{
    public :
        std::string Version;
        std::string ResponseCode;
        std::string ContentType;
        std::string Connection;
        std::vector<char> Body;
        int clientSocket;
        int responseFd;
        bool ended;
        size_t iterations;


        HttpResponse(int clientSocket, int fd, HttpRequest* req) : Version("HTTP/1.1"), ResponseCode("200 OK"), ContentType(WhatContentType(req->uri)), Connection("close"), clientSocket(clientSocket), responseFd(fd), ended(false) {

            std::string headers = "HTTP/1.1 200 OK\r\n"
            // "Content-Type: " + ContentType + "\r\n"
            "Connection: keep-alive\r\n"
            "Transfer-Encoding: chunked\r\n";

            send(clientSocket, headers.c_str(), headers.size(), 0);


            iterations = 0;
        }
        ~HttpResponse() {close(responseFd);}
        void sendingResponse(long buffSize);
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

