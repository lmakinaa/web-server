#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>



class ErrorClass400 : public std::exception {
    public:
        const char* what() const throw(){
            return "HTTP/1.1 400\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n" 
                "\r\n"
                "<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr></body></html>";
        }
};


class Succes201 : public std::exception {
    public:
        const char* what() const throw(){
            return "HTTP/1.1 201\r\n"
                "Content-Type: text/html\r\n"
                "Connection: keep-alive\r\n"
                "\r\n"
                "<html><head><title>201 Created</title></head><body><center><h1>201 Created</h1></center><hr></body></html>";
        }
};
class HttpRequest {
private:

    std::string method ,uri, version, boundary;
    double content_length, chunk_size, bodyRead;
    std::map<std::string, std::string> headers;

public:
    ErrorClass400 Error400;
    Succes201 Created201;
    void PerformChecks(void);
    void ParseRequest(int client_fd);
    void ParseFirstLine(std::string line);
    void ParseHeaders(std::string line);
    void ParseBody(std::string line);

public:
    void SetMethod(std::string method) { this->method = method; }
    void SetUri(std::string uri) { this->uri = uri; }
    void SetVersion(std::string version) { this->version = version; }
    void SetHeader(std::string key, std::string value) { this->headers[key] = value; }
    void SetBoundary(std::string boundary) { this->boundary = boundary; }
    void SetContentLength(std::string content_length) { this->content_length = std::stod(content_length); }
    void SetChunkSize(double chunk_size) { this->chunk_size = chunk_size; }
    void SetBodyRead(double bodyRead) { this->bodyRead = bodyRead; }
    
    std::string GetMethod() { return this->method; }
    std::string GetUri() { return this->uri; }
    std::string GetVersion() { return this->version; }
    std::string GetHeader(std::string key) { this->headers[key]; }
    std::string GetBoundary() { return this->boundary; }
    double GetContentLength() { return this->content_length; }
    double GetChunkSize() { return this->chunk_size; }
    double GetBodyRead() { return this->bodyRead; }
   

};

std::ostream& operator<<(std::ostream& os, HttpRequest& req);


#endif