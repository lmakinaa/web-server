#include "CGI.hpp"

static void closePipe(int fds[2])
{
    close(fds[0]);
    close(fds[1]);
}

int CGI::responseCGI(HttpRequest* req, int bodyFd) {

    // bodyFd will be 0 if we get request to a .php or .py file --> it should be protected

    int outputPipe[2];

    if (pipe(outputPipe) == -1) {
        if (M_DEBUG)
            perror("pipe(2)");
        throw ErrorStatus(503, "pipe failed in responseCGI");
    }

    char *argv[] = {
        // const_cast<char*>("/usr/bin/python3"),
        const_cast<char*>("/Users/ijaija/merge/www/server-cgis/php-cgi"),
        NULL
    };

    std::string scriptPath = req->uri; // to replace this section
    std::string scriptName = req->uri;
    size_t queryPos = scriptPath.find('?');
    if (queryPos != std::string::npos) {
        scriptPath = scriptPath.substr(0, queryPos);
        scriptName = scriptName.substr(0, queryPos);
    }

    std::cerr << scriptPath << '\n';
    std::cerr << "script name: " << scriptName << '\n';


    int pid = fork();
    if (!pid)
    {
        dup2(outputPipe[1], 1);
        dup2(bodyFd, 0);

        close(bodyFd);
        closePipe(outputPipe);

        setenv("SCRIPT_FILENAME", scriptPath.c_str(), 1);
        setenv("REDIRECT_STATUS", "200", 1);
        setenv("CONTENT_TYPE", req->getHeader("Content-Type").substr(0, req->getHeader("Content-Type").size() - 2).c_str(), 1);
        setenv("REQUEST_METHOD", req->method.c_str(), 1);
        setenv("HTTP_COOKIE", req->getHeader("Cookie").c_str(), 1);
        setenv("SESSION_ID", getSessionIdFromRequest(req->getHeader("Cookie")).c_str(), 1);

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
            throw ErrorStatus(503, "fork failed in responseCGI");
        }
        
        close(outputPipe[1]);
        close(bodyFd);
        KQueue::setFdNonBlock(outputPipe[0]);
    }

    return outputPipe[0];
}

