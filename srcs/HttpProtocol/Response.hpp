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
    public :
        std::string Version;
        std::string ResponseCode;
        std::string ContentType;
        std::string Connection;
        std::vector<char> Body;
        int clientSocket;
        int responseFd;
        bool ended;


        HttpResponse(int clientSocket, int fd, std::string ContentType) : Version("HTTP/1.1"), ResponseCode("200 OK"), ContentType(ContentType), Connection("close"), clientSocket(clientSocket), responseFd(fd), ended(false) {
            std::cerr << "tconstructa\n";
            std::string headers = "HTTP/1.1 200\r\nContent-Type: " + ContentType + "\r\nConnection: keep-alive\r\nTransfer-Encoding: chunked\r\n\r\n";
            send(clientSocket, headers.c_str(), headers.size(), 0);
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

