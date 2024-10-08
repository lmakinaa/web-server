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

