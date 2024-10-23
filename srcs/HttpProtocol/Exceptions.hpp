#pragma once


#include <iostream>
#include <exception>


#ifndef M_DEBUG
# define M_DEBUG 1
#endif


class ErrorStatus;
class SuccessStatus;


class ErrorStatus {
public:
    // You can Pass NUll to debugMsg
    ErrorStatus(int errorCode, const char* debugMsg)
        : errorCode (errorCode)
        , headers ("HTTP/1.1 " + std::to_string(errorCode) + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n")
    {
        if (M_DEBUG && debugMsg)
            std::cerr << debugMsg << "\n";
        switch (errorCode)
        {
            case 500:
                statusMessage = headers + "<html><head><title>500 Internal Error</title></head><body><center><h1>500 Internal Error</h1></center><hr></body></html>";
                break;
            case 400:
                statusMessage = headers + "<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr></body></html>";
                break;
            case 404:
                statusMessage = headers + "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr></body></html>";
                break;
            // Add more status codes
            // ..
            default:
                M_DEBUG && std::cerr << "Error: Unknown success code\n";
                statusMessage = headers + "<html><head><title>500 Internal Error</title></head><body><center><h1>500 Internal Error</h1></center><hr></body></html>";
                break;
        }
    }
    const char* what() const throw() {return (statusMessage.c_str());}
    int errorCode;
    const std::string headers;
    std::string statusMessage;
};

class SuccessStatus {
public:
    // You can Pass NUll to debugMsg
    SuccessStatus(int successCode, const char* debugMsg)
        : successCode (successCode)
        , headers ("HTTP/1.1 " + std::to_string(successCode) + "\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n")
    {
        if (M_DEBUG && debugMsg)
            std::cerr << debugMsg << "\n";
        switch (successCode)
        {
            case 201:
                statusMessage = headers + "<html><head><title>201 Created</title></head><body><center><h1>201 Created</h1></center><hr></body></html>";
                break;
            // Add more status codes
            // ..
            default:
                M_DEBUG && std::cerr << "Error: Unknown success code\n";
                throw ErrorStatus(500, NULL);
                break;
        }
    }
    const char* what() const throw() {return (statusMessage.c_str());}
    int successCode;
    const std::string headers;
    std::string statusMessage;
};
