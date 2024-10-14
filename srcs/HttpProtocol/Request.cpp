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
    std::vector<std::string> validMethods;

    validMethods.push_back("POST");
    validMethods.push_back("GET");
    validMethods.push_back("DELETE");
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (std::getline(iss, token, ' ')) {
        tokens.push_back(strtrim(token));
    }

    // if (tokens.size() != 3) {
    //     throw HttpRequest::Error400;
    // }

    // if (std::find(validMethods.begin(), validMethods.end(), tokens[0]) == validMethods.end()) {
    //     throw HttpRequest::Error400;
    // }

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
        throw HttpRequest::Error500;
}

void HttpRequest::ParseBody(std::string line)
{
    // if (line != boundary + "--" && content_length > 0)
    // {
    std::ofstream file(bodyFile, std::ios::app);
    if (file.is_open())
    {
        file.write(line.c_str(), line.size());
    }
    else
        throw HttpRequest::Error500;
    // }
    // else if (line != "0\r")
    // {
    //     line = line.substr(0, line.size() - 1);
    //     std::ofstream file(bodyFile, std::ios::app);
    //     if (file.is_open())
    //         file.write(line.c_str(), line.size());
    //     else
    //         throw HttpRequest::Error500;
    // }
}

enum ParseState{
    FirstLine,
    Headers,
    Body
};

void HttpRequest::ParseRequest(char *request)
{
    std::string line;
    ParseState state = FirstLine;
    
    std::stringstream tokensStream(request);

    while (std::getline(tokensStream, line, '\n'))
    {
        bool LINE_WITH_NO_CRLF = (line.size() < 1 || line.substr(line.size() - 1) != "\r");
        // if(LINE_WITH_NO_CRLF)
        //     throw HttpRequest::Error400;
        if (line == "\r" && state == Headers)
        {
            if (method == "POST" || method == "DELETE")
            {
                state = Body;
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
                ParseBody(line);
        }
        // std::cout << line << std::endl;
    }
    
}

void HttpRequest::PerformChecks(void){

    bool ValidMethod = false;
    std::string methods[3] = {"POST", "GET", "DELETE"};

    for (int i = 0; i < 3; i++)
        if (this->GetMethod() == methods[i])
            ValidMethod = true;
    if (!ValidMethod)
        throw HttpRequest::Error400;
    
    if (this->GetUri()[0] != '/' || this->GetVersion() != "HTTP/1.1" )
        throw HttpRequest::Error400;
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
    char buffer[1000000] = {0};
    do{
        read_bytes += read(fd, buffer, 1000000);
        std::cout << "\033[1;31m"<< read_bytes <<"\033[0m\n";
        ParseRequest(buffer);
    } 
    while (read_bytes < content_length);
    std::cout << "<_________________Parsed Request__________>" << std::endl;
    std::cout << *this << std::endl;
    PerformChecks();
}