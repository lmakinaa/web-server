#include "Request.hpp"
#include <sstream>
#include <vector>
#include <string>
#include "../server/Server.hpp"
#include <fstream>

std::string strtrim(std::string str)
{
    unsigned long x;
    unsigned long y;
    y = 0;
    x = 0;
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

    // bool FALSE_HEADER_FORMAT = (line.find(": ") == std::string::npos);

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
    else if (key == "Transfer-Encoding")
        TransferEncoding = value;
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

enum ChunkState {
    WAITING_FOR_SIZE,
    READING_CHUNK,
    SKIPPING_EMPTY_LINE
};

ChunkState chunkState = WAITING_FOR_SIZE;

void HttpRequest::ParseRequest(char *request, size_t size)
{

    std::string line(request);
    if (line == "\r\n" && state == Headers)
    {
        if (method == "POST")
        {
            state = Body;
            generateUniqueFile();
        }
        return ;
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
            if (TransferEncoding == "chunked\r\n")
                UnchunkBody(request, size);
            else
                ParseBody(request, size);            
    }
}

void HttpRequest::UnchunkBody(char *request, size_t size)
{
   if (chunk_size == 0)
   {
        try{
            chunk_size = std::stoul(request, nullptr, 16);
            std::cout << "\033[1;32m" << request << " = " << chunk_size << "\033[0m" << std::endl;
        }
        catch(const std::exception& e){
            std::cout << "\033[1;31m" << request << "\033[0m" << std::endl;
        }
        return ;
   }
   else if (chunkPos < chunk_size)
   {
        if (size > 2)
            chunkPos+=size -2;
        else
            chunkPos+=size;
        ParseBody(request,size);
        if (chunkPos == chunk_size)
        {
            chunk_size = 0;
            chunkPos = 0;
        }
   }
//    else
//     {
//         ParseBody(request, size);
//         chunk_size = 0;
//         chunkPos = 0;
//     }
}


void HttpRequest::PerformChecks(void){

    bool ValidMethod = false;
    std::string methods[3] = {"POST", "GET", "DELETE"};

    for (int i = 0; i < 3; i++)
        if (this->GetMethod() == methods[i])
            ValidMethod = true;
    if (!ValidMethod)
        throw ErrorClass400();
    
    if (this->GetUri()[0] != '/' || this->GetVersion() != "HTTP/1.1\r\n" )
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

void HttpRequest::ReadRequest(int fd) {
    std::vector<char> crlf;
    crlf.push_back('\r');
    crlf.push_back('\n');
    const size_t buffer_size = 100000;
    char buffer[buffer_size];

    read_bytes = recv(fd, buffer, buffer_size, 0);

    if (read_bytes < 0) 
    {
        std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
        return;
    }
    if (read_bytes == 0)
    {
        isDone = true;
        return;
    }
    total_read_bytes += read_bytes;
    partial_data.insert(partial_data.end(), buffer, buffer + read_bytes);
    std::vector<char>::iterator pos;
    while ((pos = std::search(partial_data.begin(), partial_data.end(), crlf.begin(), crlf.end())) != partial_data.end())
    {
        std::vector<char> line(partial_data.begin(), pos + 2);
        ParseRequest(line.data(), line.size());
        partial_data.erase(partial_data.begin(), pos + 2);
    }
    if (total_read_bytes >= content_length)
        isDone = true;
    memset(buffer, 0, buffer_size);
    if (!partial_data.empty() && isDone)
        ParseRequest(partial_data.data(), partial_data.size());

}
