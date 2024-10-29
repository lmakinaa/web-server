#include "Response.hpp"

void HttpResponse::SetVersion(std::string value){
    this->Version = value;
}

void HttpResponse::SetResponseCode(std::string value){
    this->ResponseCode = value;
}

void HttpResponse::SetContentType(std::string value){
    this->ContentType = value;
}

void HttpResponse::SetConnection(std::string value){
    this->Connection = value;
}

void HttpResponse::SetBody(std::vector<char> value){
    this->Body.insert(this->Body.end(), value.begin(), value.end());
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
    
    // Use Switch case here
    if ( uri.rfind(".") == std::string::npos|| uri.substr(uri.rfind(".")) == ".html" || uri == "/")
        return "text/html";
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

    buffSize /= 1.75;

    std::cerr << buffSize << '\n';

    char buff[buffSize];

    int r = read(responseFd, buff, buffSize);
    if (r <= 0) {
        if (r == 0) {
            send(clientSocket, "0\r\n\r\n", 5, 0);
            ended = true;
            std::cerr << "The connection is ended and closed\n";
        }
        return;
    }


    std::stringstream tmp;
    tmp << std::hex << r;
    std::string chunkLenHex = tmp.str() + "\r\n";
    std::string fullMessage = chunkLenHex + std::string(buff, r) + "\r\n";


    if (send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0) == -1) {
        std::cerr << "Error in send: possibly client disconnect\n";
        ended = true;  // Treat send failure as a connection issue
    }
}