#pragma once

#include <iostream>

class HttpResponse{
    private :
        std::string Version;
        std::string ResponseCode;
        std::string ContentType;
        std::string Connection;
        std::string Response;
    public :
        void SetVersion(std::string value);
        void SetResponseCode(std::string value);
        void SetContentType(std::string value);
        void SetConnection(std::string value);
        void SetResponse(std::string value);

        std::string GetVersion();
        std::string GetResponseCode();
        std::string GetContentType();
        std::string GetConnection();
        std::string GetResponse();
        std::string BuildResponse();

};

std::string WhatContentType(std::string uri);

