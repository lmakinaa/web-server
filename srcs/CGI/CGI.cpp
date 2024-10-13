#include "CGI.hpp"

extern char **environ;

static void readOutput(int fd, std::string& buff)
{
    char* cbuff; 
    try {cbuff = new char[BUFF_SIZE]();} catch(std::exception&e) {return ;} // No response if fail
    size_t count = 0;
    while (1) {
        int r = read(fd, cbuff, BUFF_SIZE);

        if (r <= 0) {
            if (r == -1 && M_DEBUG)
                perror("execve(2)");
            break ; // No response
        }

        count += r;
        if (count > buff.max_size())
            break ; // No response

        buff.append(cbuff, r);
        std::memset(cbuff, 0, r + 1);
        if (r < BUFF_SIZE)
            break ;
    }
    delete[] cbuff;
}

static void closePipe(int fds[2])
{
    close(fds[0]);
    close(fds[1]);
}

void CGI::scriptToHtml(t_method reqMethod, const char* cgiPath, const char* argv[], std::string& buff, std::string& postData)
{
    int fds[2];
    int input_pipe[2];
    if (pipe(fds) == -1 || pipe(input_pipe) == -1) {
        if (M_DEBUG)
            perror("pipe(2)");
        return ; // No response
    }

    int pid = fork();
    if (!pid)
    {
        dup2(fds[1], 1);
        dup2(input_pipe[0], 0);
        
        closePipe(input_pipe);
        closePipe(fds);

        setenv("SCRIPT_FILENAME", argv[1], 1);
        setenv("REDIRECT_STATUS", "200", 1);
        setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
        setenv("REQUEST_METHOD", reqMethod == POST ? "POST":(reqMethod == GET ? "GET":"DELETE"), 1);

        (reqMethod == POST) && setenv("CONTENT_LENGTH", std::to_string(postData.length()).c_str(), 1);
        (reqMethod == GET) && setenv("QUERY_STRING", postData.c_str(), 1);
        

        if (execve(cgiPath, const_cast<char* const*>(argv), environ) == -1) {
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
            return (closePipe(fds)); // No response
        }

        write(input_pipe[1], postData.c_str(), postData.length());
        closePipe(input_pipe);
     
        if (waitpid(pid, NULL, 0) == -1)
        {
            if (M_DEBUG)
                std::perror("waitpid(2)");
            return (closePipe(fds)); // No response
        }
        else
        {
            close(fds[1]);
            readOutput(fds[0], buff);
            close(fds[0]);
        }
    }
}

// int main()
// {
//     const char* cgiPath = "/Users/ijaija/web-server/www/server-cgis/python-cgi";
//     const char* cgi = "php";
//     const char* file = "/Users/ijaija/web-server/srcs/CGI/test.py";
//     const char* argv[3];
//     argv[0] = cgi;
    

//     argv[1] = file;
//     argv[2] = NULL;

//     std::string buff;

// std::string postData = "var1=5454&var2=test&path=sds";

//     CGI::scriptToHtml(POST, cgiPath, argv, buff, postData);

//     std::cout << buff << std::endl;
//     return 0;

// }

