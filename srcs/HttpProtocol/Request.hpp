#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include "Exceptions.hpp"

enum ParseState{
    FirstLine,
    Headers,
    Body
};

class HttpRequest {
private:

    std::string method ,uri, version, boundary, bodyFile;
    long long content_length, chunk_size, bodyRead;
    std::map<std::string, std::string> headers;
    std::vector<char> partial_data;
    ParseState state;
    size_t total_read_bytes;
    ssize_t read_bytes;
    size_t readed_body;
    bool isDone;
    int iters;

public:

    void PerformChecks(void);
    void ParseRequest(char *request, size_t size);
    void ParseFirstLine(std::string line);
    void ParseHeaders(std::string line);
    void ParseBody(char *line, size_t size);


    HttpRequest() : content_length(0), chunk_size(0), bodyRead(0), state(FirstLine) , total_read_bytes(0), read_bytes(0) , isDone(false), iters(0) {
        readed_body= 0;
        partial_data.reserve(1);
    }
    void SetMethod(std::string method) { this->method = method; }
    void SetUri(std::string uri) { this->uri = uri; }
    void SetVersion(std::string version) { this->version = version; }
    void SetHeader(std::string key, std::string value) { this->headers[key] = value; }
    void SetBoundary(std::string boundary) { this->boundary = boundary; }
    void SetContentLength(std::string content_length) { this->content_length = std::stol(content_length); }
    void SetChunkSize(double chunk_size) { this->chunk_size = chunk_size; }
    void SetBodyRead(double bodyRead) { this->bodyRead = bodyRead; }
    
    std::string GetMethod() { return this->method; }
    std::string GetUri() { return this->uri; }
    std::string GetVersion() { return this->version; }
    std::string GetHeader(std::string key) { return this->headers[key]; }
    std::string GetBoundary() { return this->boundary; }
    double GetContentLength() { return this->content_length; }
    double GetChunkSize() { return this->chunk_size; }
    double GetBodyRead() { return this->bodyRead; }
    bool getIsDone() { return this->isDone; }
    std::map<std::string, std::string> GetHeaders() { return this->headers; }
    void generateUniqueFile(void);
    void ReadRequest(int fd);
    size_t getTotalReadBytes(){return this->total_read_bytes;}

};

std::ostream& operator<<(std::ostream& os, HttpRequest& req);

#endif