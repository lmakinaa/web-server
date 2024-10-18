#include "Request.hpp"
#include <sstream>
#include <vector>
#include <string>
#include "../server/Server.hpp"
#include <fstream>

std::string strtrim(std::string str)
{
    int x,y = 0;
    for (; x < str.size(); x++)
        if (str[x] != ' ' && str[x] != '\r' && str[x] != '\t')
            break;
   for (y = str.size() - 1; y >= 0; y--)
        if (str[y] != ' ' && str[y] != '\r' && str[y] != '\t')
            break;
    return str.substr(x, y - x + 1);
}

void HttpRequest::ParseFirstLine(std::string line)
{
    std::vector<std::string> tokens;
    std::stringstream tokensStream(line);
    std::string token;

    while (std::getline(tokensStream, token, ' '))
        tokens.push_back(token);

    if (tokens.size() != 3)
        throw ErrorClass400();

    SetMethod(tokens[0]);
    SetUri(tokens[1]);
    SetVersion(tokens[2]);
}

void HttpRequest::ParseHeaders(std::string line)
{
    std::string key;
    std::string value;

    bool FALSE_HEADER_FORMAT = (line.find(": ") == std::string::npos);

    // if (FALSE_HEADER_FORMAT)
    // {
    //     std::cout << "\033[1;31m"<< line <<"\033[0m\n";
    //     throw HttpRequest::Error400;
    // }

    key     = line.substr(0, line.find(": "));
    value   = line.substr(line.find(": ") + 1, line.size() - 1);

    key = strtrim(key);
    value = strtrim(value);

    if (key == "Content-Length")
        SetContentLength(value);
    else if (value.find("boundary=") != std::string::npos)
        SetBoundary(value.substr(value.find("boundary=") + 9, value.size() - 1));
    else
        SetHeader(key, value);
}

void HttpRequest::generateUniqueFile(void)
{
    static int i = 0;
    if (i == INT_MAX)
        i = 0;
    std::string file_name = "temp_" + std::to_string(i) + ".txt";
    std::ofstream file(file_name);
    if (file.is_open())
    {
        file.close();
        bodyFile = file_name;
        i++;
    }
    else
        throw ErrorClass500();
}

void HttpRequest::ParseBody(char *line, size_t size)
{
    std::ofstream file(bodyFile, std::ios::app | std::ios::binary);
    if (file.is_open())
    {
        file.write(line,size);
    }
    else
        throw ErrorClass500();
}

enum ParseState{
    FirstLine,
    Headers,
    Body
};

void HttpRequest::ParseRequest(char *request, size_t size)
{
    std::string line;
    int count = 0;
    std::stringstream tokensStream(request);

    while (count < size)
    {
        getline(tokensStream, line);
        count += line.size() + 1;
        bool LINE_WITH_NO_CRLF = (line.size() < 1 || line.substr(line.size() - 1) != "\r");
        // if(LINE_WITH_NO_CRLF)
        //     throw HttpRequest::Error400;
        if (line == "\r" && state == Headers)
        {
            if (method == "POST" || method == "DELETE")
            {
                state = Body;
                request += count;
                generateUniqueFile();
            }
            else
                break;
        }
        switch (state)
        {
            case FirstLine:
                ParseFirstLine(line);
                state = Headers;
                break;
            case Headers:
                ParseHeaders(line);
                break;
            case Body:
                ParseBody(request, size - count);
                count  = size;
        }
    }

}

void HttpRequest::PerformChecks(void){

    bool ValidMethod = false;
    std::string methods[3] = {"POST", "GET", "DELETE"};

    for (int i = 0; i < 3; i++)
        if (this->GetMethod() == methods[i])
            ValidMethod = true;
    if (!ValidMethod)
        throw ErrorClass400();
    
    if (this->GetUri()[0] != '/' || this->GetVersion() != "HTTP/1.1\r" )
        throw ErrorClass400();
}

std::ostream& operator<<(std::ostream& os, HttpRequest& req)
{
    os << "Method: " << req.GetMethod() << std::endl;
    os << "Uri: " << req.GetUri() << std::endl;
    os << "Version: " << req.GetVersion() << std::endl;
    os << "Content-Length: " << req.GetContentLength() << std::endl;
    os << "Boundary: " << req.GetBoundary() << std::endl;
    os << "Headers: " << std::endl;
    std::map<std::string, std::string> print = req.GetHeaders();

    for (std::map<std::string, std::string>::iterator it = print.begin(); it != print.end(); it++)
        os << it->first << ": " << it->second << std::endl;
    return os;
}

void HttpRequest::ReadRequest(int fd){
    size_t read_bytes = 0;
    char *buffer = (char *)calloc(5000000, sizeof(char));
    size_t result;

    do{
        result = read(fd, buffer + read_bytes, 5000000 - read_bytes);
        if (result < 0) {
            std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
            free(buffer);
            return;
        }
        read_bytes += result;
    }
    while(result > 0 && result == 5000000 - read_bytes);

    ParseRequest(buffer, read_bytes);
    std::cout << "<_________________Parsed Request__________>" << std::endl;
    std::cout << *this << std::endl;
    PerformChecks();
}