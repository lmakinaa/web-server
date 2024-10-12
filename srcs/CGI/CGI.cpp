#include "CGI.hpp"

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

void CGI::scriptToHtml(const char* cgiPath, const char* argv[], std::string& buff)
{
    int fds[2];
    if (pipe(fds) == -1) {
        if (M_DEBUG)
            perror("pipe(2)");
        return ; // No reponse
    }

    int pid = fork();
    if (!pid)
    {
        dup2(fds[1], 1);
        if (execve(cgiPath, const_cast<char* const*>(argv), NULL) == -1) {
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
        if (waitpid(pid, NULL, 0) == -1)
        {
            if (M_DEBUG)
                perror("waitpid(2)");
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
//     const char* cgiPath = "/usr/bin/php";
//     const char* cgi = "php";
//     const char* file = "/Users/ijaija/web-server/srcs/CGI/test.php";
//     const char* argv[3];
//     argv[0] = cgi;
    

//     argv[1] = file;
//     argv[2] = NULL;

//     std::string buff;

//     CGI::scriptToHtml(cgiPath, argv, buff);

//     std::cout << buff << std::endl;
//     return 0;

// }