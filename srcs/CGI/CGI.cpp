#include "CGI.hpp"

std::string  generateRandomFileName(const std::string path, const std::string extension) {

    std::srand(static_cast<unsigned int>(std::time(0)));

    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);

    char timeBuffer[15]; // "YYYYMMDDHHMMSS"
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y%m%d%H%M%S", localTime);

    std::ostringstream oss;
    if (!path.empty()) {
        oss << path;
        if (path[path.size() - 1] != '/') {
            oss << "/";
        }
    }

    oss << ".";

    oss << timeBuffer;
    oss << "_" << (std::rand() % 10000);

    if (!extension.empty()) {
        oss << "." << extension;
    }

    return oss.str();
}


// static void closePipe(int fds[2])
// {
//     close(fds[0]);
//     close(fds[1]);
// }

int CGI::responseCGI(HttpRequest* req, int bodyFd) {

    // bodyFd will be 0 if we get request to a .php or .py file --> it should be protected
    // custom error pages
    Directive *error_page = NULL;

    std::map<std::string, Directive>::iterator eit = req->s->directives.find("error_page");
    if ( eit != req->s->directives.end())
        error_page = &(eit->second);

    // int outputPipe[2];

    // if (pipe(outputPipe) == -1) {
    //     if (M_DEBUG)
    //         perror("pipe(2)");
    //     throw ErrorStatus(503, "pipe failed in responseCGI", error_page);
    // }

    std::string fileName = generateRandomFileName("/tmp/", "webserv");
    
    int outputFile = open(fileName.c_str(), O_CREAT | O_WRONLY, 0644);
    if (outputFile == -1) {
        if (M_DEBUG)
            perror("pipe(2)");
        throw ErrorStatus(503, "open(2) failed in responseCGI", error_page);
    }

    std::string scriptName = req->uri;
    std::string cgiPath;
    size_t queryPos = scriptName.find('?');
    if (queryPos != std::string::npos) 
        scriptName = scriptName.substr(0, queryPos);
    if (scriptName.find(".py") != std::string::npos)
        cgiPath = "/usr/bin/python3";
    else
        cgiPath = "/Users/ijaija/merge/www/cgi-bin/php-cgi";

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
        dup2(outputFile, 1);
        dup2(bodyFd, 0);

        close(outputFile);
        close(bodyFd);

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
            close(outputFile);
            close(bodyFd);
            throw ErrorStatus(503, "fork failed in responseCGI", error_page);
        }
        req->cgiPid = pid;
        close(outputFile);
        outputFile = open(fileName.c_str(), O_RDONLY, 0644);
        unlink(fileName.c_str());
        KQueue::setFdNonBlock(outputFile);
    }

    return outputFile;
}

