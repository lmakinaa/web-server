#include "CGI.hpp"

static void closePipe(int fds[2])
{
    close(fds[0]);
    close(fds[1]);
}

int CGI::responseCGI(HttpRequest* req, int bodyFd, Location *location) {

    // bodyFd will be 0 if we get request to a .php or .py file --> it should be protected
    // custom error pages
    Directive *error_page = NULL;

    std::map<std::string, Directive>::iterator eit = req->s->directives.find("error_page");
    if ( eit != req->s->directives.end())
        error_page = &(eit->second);

    int outputPipe[2];

    if (pipe(outputPipe) == -1) {
        if (M_DEBUG)
            perror("pipe(2)");
        throw ErrorStatus(503, "pipe failed in responseCGI", error_page);
    }

    std::string scriptName = req->uri;
    std::string cgiPath = "";
    size_t queryPos = scriptName.find('?');
    if (queryPos != std::string::npos) 
        scriptName = scriptName.substr(0, queryPos);
    if (scriptName.find(".py") != std::string::npos)
    {
        if (location && location->directives.find("py-cgi") != location->directives.end())
            cgiPath = location->directives["py-cgi"].values[0];
        else if (location)
            ErrorStatus(404, "Python Cgi path not found", error_page);
    }
    else
    {
        if (location && location->directives.find("php-cgi") != location->directives.end())
            cgiPath = location->directives["php-cgi"].values[0];
        else if (location)
            ErrorStatus(404, "php Cgi path not found", error_page);
    }

    char *argv[] = {
        const_cast<char*>(cgiPath.c_str()),
        const_cast<char*>(scriptName.c_str()),
        NULL
    };

    // char *argv[] = {
    //     const_cast<char*>("/usr/bin/python3"),
    //     const_cast<char*>("/Users/ijaija/merge/www/html/test.py"),
    //     NULL
    // };



    int pid = fork();
    if (!pid)
    {
        dup2(outputPipe[1], 1);
        dup2(bodyFd, 0);

        close(bodyFd);
        closePipe(outputPipe);

        std::string cookie = req->getHeader("Cookie");
        if (cookie != "")
            cookie.insert(cookie.size() - 2, ";");

        setenv("SCRIPT_FILENAME", scriptName.c_str(), 1);
        setenv("REDIRECT_STATUS", "200", 1);
        setenv("CONTENT_TYPE", req->getHeader("Content-Type").substr(0, req->getHeader("Content-Type").size() - 2).c_str(), 1);
        setenv("REQUEST_METHOD", req->method.c_str(), 1);
        setenv("HTTP_COOKIE", cookie.c_str(), 1);

        (req->method == "POST") && setenv("CONTENT_LENGTH", std::to_string(req->content_length).c_str(), 1);
        if (queryPos != std::string::npos)
            setenv("QUERY_STRING", req->uri.substr(queryPos + 1).c_str(), 1);
        
        if (execve(argv[0], const_cast<char* const*>(argv), environ) == -1) {
            if (M_DEBUG)
                perror("execve(2)");
            exit(1);
        }
    }
    else
    {
        if (pid == -1) {
            if (M_DEBUG)
                perror("fork(2)");
            closePipe(outputPipe);
            close(bodyFd);
            throw ErrorStatus(503, "fork failed in responseCGI", error_page);
        }
        
        close(outputPipe[1]);
        KQueue::setFdNonBlock(outputPipe[0]);
    }

    return outputPipe[0];
}

