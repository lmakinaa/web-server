#include "Exceptions.hpp"

void ErrorStatus::setErrorMessage()
{
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

ErrorStatus::ErrorStatus(int clienSocket, int errorCode, const char* debugMsg)
    : clientSock (clienSocket)
    , errorCode (errorCode)
    , headers ("HTTP/1.1 " + std::to_string(errorCode) + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n")
{
    if (M_DEBUG && debugMsg)
        std::cerr << debugMsg << "\n";
    setErrorMessage();
}

ErrorStatus::ErrorStatus(int errorCode, const char* debugMsg)
    : clientSock (-1)
    , errorCode (errorCode)
    , headers ("HTTP/1.1 " + std::to_string(errorCode) + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n")
{
    if (M_DEBUG && debugMsg)
        std::cerr << debugMsg << "\n";
    setErrorMessage();
}

// Success
void SuccessStatus::setSuccessMessage()
{
    switch (successCode)
    {
        case 201:
            statusMessage = headers + "<html><head><title>201 Created</title></head><body><center><h1>201 Created</h1></center><hr></body></html>";
            break;
        // Add more status codes
        // ..
        default:
            M_DEBUG && std::cerr << "Error: Unknown success code\n";
            throw ErrorStatus(clientSock, 500, NULL);
            break;
    }
}

SuccessStatus::SuccessStatus(int successCode, const char* debugMsg)
    : clientSock (-1)
    , successCode (successCode)
    , headers ("HTTP/1.1 " + std::to_string(successCode) + "\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n")
{
    if (M_DEBUG && debugMsg)
        std::cerr << debugMsg << "\n";
    setSuccessMessage();
}

SuccessStatus::SuccessStatus(int clienSocket, int successCode, const char* debugMsg)
    : clientSock (clienSocket)
    , successCode (successCode)
    , headers ("HTTP/1.1 " + std::to_string(successCode) + "\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n")
{
    if (M_DEBUG && debugMsg)
        std::cerr << debugMsg << "\n";
    setSuccessMessage();
}