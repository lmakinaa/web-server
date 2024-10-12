#include "Response.hpp"

void HttpResponse::SetVersion(std::string value){
    this->Version = value;
}

void HttpResponse::SetResponseCode(std::string value){
    this->ResponseCode = value;
}

void HttpResponse::SetContentType(std::string value){
    this->ContentType = "Content-Type: " + value + "\r\n";
}

void HttpResponse::SetConnection(std::string value){
    this->Connection = "Connection: " + value + "\r\n";
}

void HttpResponse::SetResponse(std::string value){
    this->Response += value;
}

std::string HttpResponse::BuildResponse() {
    return Version + " " + ResponseCode + "\r\n" + ContentType + Connection + "\r\n" + Response;
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

std::string HttpResponse::GetResponse(){
    return this->Response;
}


std::string WhatContentType(std::string uri){

    if (uri.substr(uri.rfind(".")) == ".html")
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