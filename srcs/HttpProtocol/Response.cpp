#include "Response.hpp"

std::string strToLower(std::string s)
{
    std::string res("");
    
    for (std::string::iterator i = s.begin(); i < s.end(); i++) {
        res += std::tolower(*i);
    }
    return res;
}

const std::vector<char> HttpResponse::BuildResponse() {
    std::string temp = this->Version + " " + this->ResponseCode + "\r\n" + "Content-Type: " + this->ContentType + "\r\n" + "Connection: " + this->Connection + "\r\n" + "\r\n" ;
    std::vector<char> response(temp.begin(), temp.end());
    response.insert(response.end(), Body.begin(), Body.end());
    return response;
}

std::string HttpResponse::GetVersion(){
    return this->Version;
}

std::string HttpResponse::GetResponseCode(){
    return this->ResponseCode;
}

std::string HttpResponse::GetContentType(){
    return this->ContentType;
}

std::string HttpResponse::GetConnection(){
    return this->Connection;
}

const std::vector<char>& HttpResponse::GetBody(){
    return this->Body;
}


std::string WhatContentType(std::string uri) {
    
    if ( uri.rfind(".") == std::string::npos|| uri.substr(uri.rfind(".")) == ".html" || uri == "/" ||
     uri.substr(uri.rfind(".")) == ".php" || uri.substr(uri.rfind(".")) == ".py" || strncmp(uri.substr(uri.rfind(".")).c_str(),".php?",5) == 0 || strncmp(uri.substr(uri.rfind(".")).c_str(),".py?",4) == 0)
    {
        return "text/html";
    }
    else if (uri.substr(uri.rfind(".")) == ".css")
        return "text/css";
    else if (uri.substr(uri.rfind(".")) == ".js")
        return "text/javascript";
    else if (uri.substr(uri.rfind(".")) == ".jpg")
        return "image/jpeg";
    else if (uri.substr(uri.rfind(".")) == ".jpeg")
        return "image/jpeg";
    else if (uri.substr(uri.rfind(".")) == ".png")
        return "image/png";
    else if (uri.substr(uri.rfind(".")) == ".gif")
        return "image/gif";
    else if (uri.substr(uri.rfind(".")) == ".bmp")
        return "image/bmp";
    else if (uri.substr(uri.rfind(".")) == ".ico")
        return "image/x-icon";
    else if (uri.substr(uri.rfind(".")) == ".svg")
        return "image/svg+xml";
    else if (uri.substr(uri.rfind(".")) == ".mp3")
        return "audio/mpeg";
    else if (uri.substr(uri.rfind(".")) == ".wav")
        return "audio/wav";
    else if (uri.substr(uri.rfind(".")) == ".mp4")
        return "video/mp4";
    else if (uri.substr(uri.rfind(".")) == ".avi")
        return "video/x-msvideo";
    else
        return "application/octet-stream";
}


void HttpResponse::sendingResponse(long buffSize) {

    buffSize *= 0.80; // I wont use the whole available buffSize to decrease the load on it

    char bf[buffSize];

    (iterations == 0) && lseek(responseFd, 0, SEEK_SET);
    int r = read(responseFd, bf, buffSize);
    
    std::cerr << "read bytes: " << r << '\n';
    if (r <= 0) {
        if (r == 0) {
            send(clientSocket, "0\r\n\r\n", 5, 0);
            ended = true;
            M_DEBUG && std::cerr << "The connection is ended\n";
        }
        // When using non block mode, r may returns -1 if there is no data in the fd
        // and it will normally wait but here it'll return -1
        return;
    }

    char *buff = bf;
    if (iterations == 0) {
        std::string tmp(bf);
        size_t p = tmp.find("\r\n\r\n");
        if (p != std::string::npos) {
            p += 4;
            tmp = tmp.substr(0, p);
            r -= p;
            buff = bf + p;
            send(clientSocket, tmp.c_str(), tmp.size(), 0);
        }
    }

    std::stringstream tmp;
    tmp << std::hex << r;

    std::string chunkLenHex = tmp.str() + "\r\n";
    std::string fullMessage = chunkLenHex + std::string(buff, r) + "\r\n";

    if (send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0) == -1) {
        if (M_DEBUG) {
            std::cerr << "the client probably disconnected -- ";
            perror("send(2)");
        }
        ended = true; // Treat send failure as a connection issue, I wont throw because there is no one to receive the error code
    }

    iterations++;
}