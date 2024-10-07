#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

class HttpRequest {
private:
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
    std::string user_agent;
    std::string accept; 

public:
    void SetMethod(std::string method) { this->method = method; }
    void SetUri(std::string uri) { this->uri = uri; }
    void SetVersion(std::string version) { this->version = version; }
    void SetHost(std::string host) { this->host = host; }
    void SetUserAgent(std::string user_agent) { this->user_agent = user_agent; }
    void SetAccept(std::string accept) { this->accept = accept; }

    std::string GetMethod() { return this->method; }
    std::string GetUri() { return this->uri; }
    std::string GetVersion() { return this->version; }
    std::string GetHost() { return this->host; }
    std::string GetUserAgent() { return this->user_agent; }
    std::string GetAccept() { return this->accept; }
    void PerformChecks(void);
    void ParseRequest(std::string request);

    class Error400 : public std::exception {
        public:
            const char* what() const throw(){
                return "HTTP/1.1 400\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n" 
                    "\r\n"
                    "<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr></body></html>";
            }
    };

};

std::ostream& operator<<(std::ostream& os, HttpRequest& req);


#endif