#include "Request.hpp"
#include <sstream>

void HttpRequest::ParseRequest(std::string request)
{
    std::string line;
    std::sstream iss(request);
    while (std::getline(iss, line))
    {
        if (line.find("GET") != std::string::npos)
        {
            SetMethod("GET");
            SetUri(line.substr(4, line.find("HTTP") - 5));
           SetVersion(line.substr(line.find("HTTP")));
        }
        else if (line.find("Host") != std::string::npos)
        {
            SetHost(line.substr(6));
        }
        else if (line.find("User-Agent") != std::string::npos)
        {
            SetUserAgent(line.substr(12));
        }
        else if (line.find("Accept") != std::string::npos)
        {
            SetAccept(line.substr(8));
        }
    }

}

std::ostream& operator<<(std::ostream& os, HttpRequest& req)
{
    os << "Method: " << req.GetMethod() << std::endl;
    os << "Uri: " << req.GetUri() << std::endl;
    os << "Version: " << req.GetVersion() << std::endl;
    os << "Host: " << req.GetHost() << std::endl;
    os << "User-Agent: " << req.GetUserAgent() << std::endl;
    os << "Accept: " << req.GetAccept() << std::endl;
    return os;
}