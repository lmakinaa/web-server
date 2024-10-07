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
    std::string methods[3] = {"GET", "POST", "DELETE"};
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

    class Error400 : public std::exception{
        public:
            const char* what() const throw(){
                return "<html><body><h1>400 Bad Request</h1></body></html>\r\n";
            }
    };

    // class Error402 : public std::exception{
    //     public:
    //         const char* what() const throw(){
    //             return "<html><body><h1>400 Bad Request</h1></body></html>\r\n";
    //         }
    // };

};

std::ostream& operator<<(std::ostream& os, HttpRequest& req);



#endif