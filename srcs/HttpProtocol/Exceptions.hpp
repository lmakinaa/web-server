#pragma once


#include <iostream>
#include <exception>
#include <unistd.h>
#include <sys/socket.h>


#ifndef M_DEBUG
# define M_DEBUG 1
#endif


class ErrorStatus;
class SuccessStatus;


class ErrorStatus {
public:
    // You can Pass NUll to debugMsg
    ErrorStatus(int clienSocket, int errorCode, const char* debugMsg);
    ErrorStatus(int errorCode, const char* debugMsg);
    void setErrorMessage();
    ~ErrorStatus() {
        if (clientSock != -1)
            close(clientSock);
    }
    void sendError() const throw() {
        if (clientSock != -1)
            send(clientSock, statusMessage.c_str(), statusMessage.size(), 0);
    }
    const char* what() const throw() {return (statusMessage.c_str());}
    int clientSock;
    int errorCode;
    const std::string headers;
    std::string statusMessage;
};

class SuccessStatus {
public:
    // You can Pass NUll to debugMsg
    SuccessStatus(int clienSocket, int successCode, const char* debugMsg);
    SuccessStatus(int successCode, const char* debugMsg);
    void setSuccessMessage();
    ~SuccessStatus() {
        if (clientSock != -1)
            close(clientSock);
    }
    void sendError() const throw() {
        if (clientSock != -1)
            send(clientSock, statusMessage.c_str(), statusMessage.size(), 0);
    }
    const char* what() const throw() {return (statusMessage.c_str());}
    int clientSock;
    int successCode;
    const std::string headers;
    std::string statusMessage;
};
