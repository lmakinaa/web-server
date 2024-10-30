#include "CGI.hpp"

char** CGI::setupCGIEnvironment(HttpRequest* req) {
    std::vector<std::string> envVars;
    
    std::string documentRoot = "/Users/ijaija/merge"; 
    std::string scriptPath = req->uri;
    std::string scriptName = req->uri;
    
    size_t queryPos = scriptPath.find('?');
    if (queryPos != std::string::npos) {
        scriptPath = documentRoot + scriptPath.substr(0, queryPos);
        scriptName = scriptName.substr(0, queryPos);
    }
    else
        scriptPath = documentRoot + scriptPath;

    std::cerr << scriptPath << '\n';

    envVars.push_back("REDIRECT_STATUS=200");
    envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envVars.push_back("SERVER_SOFTWARE=CustomWebServer/1.0");
    envVars.push_back("SERVER_NAME=localhost");
    envVars.push_back("SERVER_PORT=8080");
    envVars.push_back("REQUEST_METHOD=" + req->method);
    envVars.push_back("SCRIPT_FILENAME=" + scriptPath);
    envVars.push_back("SCRIPT_NAME=" + scriptName);
    envVars.push_back("DOCUMENT_ROOT=" + documentRoot);
    envVars.push_back("PATH_TRANSLATED=" + scriptPath);
    // envVars.push_back("CONTENT_TYPE=multipart/form-data; boundary=" + req->boundary);

    
    if (queryPos != std::string::npos) {
        envVars.push_back("QUERY_STRING=" + req->uri.substr(queryPos + 1));
    } else {
        envVars.push_back("QUERY_STRING=");
    }

    if (req->method == "POST") {
        envVars.push_back("CONTENT_LENGTH=" + std::to_string(req->content_length));
        std::string contentType = req->getHeader("Content-Type"); // this is not parsed
        // if (!contentType.empty()) {
            std::cerr << req->boundary.substr(0, req->boundary.size() - 2) << "----" << req->boundary.substr(0, req->boundary.size() - 2).size() << '\n';
            envVars.push_back("CONTENT_TYPE=multipart/form-data; boundary=" + req->boundary.substr(0, req->boundary.size() - 2));
        // }
    }

    char** env = new char*[envVars.size() + 1];
    for (size_t i = 0; i < envVars.size(); ++i) {
        env[i] = strdup(envVars[i].c_str());
    }
    env[envVars.size()] = NULL;

    for (size_t i = 0; env[i] != nullptr; ++i) {
        std::cerr << env[i] << '\n';
    }

    return env;
}
int CGI::responseCGI(HttpRequest* req, int BodyFd) {
    int pipe_in[2];
    int pipe_out[2];
    
    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) {
        throw std::runtime_error("Failed to create pipes");
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(pipe_in[0]); close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);
        throw std::runtime_error("Fork failed");
    }

    if (pid == 0) {
        try {
            close(pipe_in[1]);
            close(pipe_out[0]);

            dup2(pipe_in[0], STDIN_FILENO);
            dup2(pipe_out[1], STDOUT_FILENO);

            close(pipe_in[0]);
            close(pipe_out[1]);

            char** env = setupCGIEnvironment(req);

            char* const args[] = {
                const_cast<char*>("/Users/ijaija/merge/www/server-cgis/php-cgi"),
                nullptr
            };

            execve(args[0], args, env);

            for (int i = 0; env[i] != nullptr; ++i) {
                free(env[i]);
            }
            delete[] env;
            
            throw std::runtime_error("Failed to execute CGI script");
        }
        catch (const std::exception& e) {
            exit(1);
        }
    }

    close(pipe_in[0]);
    close(pipe_out[1]);

    if (req->method == "POST" && BodyFd != -1) {
        char buffer[4096];
        ssize_t bytes_read;
        while ((bytes_read = read(BodyFd, buffer, sizeof(buffer))) > 0) {
            if (write(pipe_in[1], buffer, bytes_read) == -1)
                perror("write(2)");
        }
    }

    close(pipe_in[1]);

    return pipe_out[0];
}

