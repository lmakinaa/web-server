#include "Request.hpp"
#include <sstream>
#include <vector>
#include <string>
#include "../server/Server.hpp"
#include <fstream>

void HttpRequest::ParseFirstLine(std::string line)
{
    
    for (int i = 0; i < 3; i++)
    {
        if (line.find(" ") == std::string::npos && i != 2)
            throw HttpRequest::Error400;
        else
        {
            std::string token = line.substr(0, line.find(" "));
            if (i == 0)
                SetMethod(token);
            else if (i == 1)
                SetUri(token);
            else if (i == 2)
                SetVersion(token);
            line = line.substr(line.find(" ") + 1);
        }
    }

    
}

void HttpRequest::ParseHeaders(std::string line)
{
    std::string key;
    std::string value;

    bool FALSE_HEADER_FORMAT = (line.find(": ") == std::string::npos);

    if (FALSE_HEADER_FORMAT)
        throw HttpRequest::Error400;

    key     = line.substr(0, line.find(": "));
    value   = line.substr(line.find(": ") + 1, line.size() - 1);

    if (key == "Content-Length")
        SetContentLength(value);
    else
        SetHeader(key, value);
}

void HttpRequest::ParseBody(std::string line)
{
    
}

enum ParseState{
    FirstLine,
    Headers,
    Body
};

void HttpRequest::ParseRequest(int client_fd)
{
    char request[1024];
    std::string line;
    ParseState state = ParseState::FirstLine;

    while (read(client_fd, request, 1024))
    {
        std::stringstream tokensStream(request);
    
        while (std::getline(tokensStream, line))
        {
            bool LINE_WITH_NO_CRLF = (line.size() < 1 || line.substr(line.size() - 1) != "\r");

            if(LINE_WITH_NO_CRLF)
                throw HttpRequest::Error400;
            if (line == "\r")
                state = ParseState::Body;

            switch (state)
            {
                case ParseState::FirstLine:
                    ParseFirstLine(line);
                    state = ParseState::Headers;
                    break;
                case ParseState::Headers:
                    ParseHeaders(line);
                    break;
                case ParseState::Body:
                    ParseBody(line);
            }
        }
    }

    PerformChecks();
}

void HttpRequest::PerformChecks(void){

    bool ValidMethod = false;
    std::string methods[3] = {"POST", "GET", "DELETE"};
    for (int i = 0; i < 3; i++)
        if (this->GetMethod() == methods[i])
            ValidMethod = true;
    if (!ValidMethod)
        throw HttpRequest::Error400;
    if (this->GetUri()[0] != '/' || this->GetVersion() != "HTTP/1.1\r" )
        throw HttpRequest::Error400;
}

std::ostream& operator<<(std::ostream& os, HttpRequest& req)
{
    os << "Method: " << req.GetMethod() << std::endl;
    os << "Uri: " << req.GetUri() << std::endl;
    os << "Version: " << req.GetVersion() << std::endl;
    os << "Host: " << req.GetHost() << std::endl;
    os << "HostPort: " << req.GetHostPort() << std::endl;
    os << "User-Agent: " << req.GetUserAgent() << std::endl;
    os << "Accept: " << req.GetAccept() << std::endl;
    os << "Content-Length: " << req.GetContentLength() << std::endl;
    os << "--------------------------------" << std::endl;
    return os;
}