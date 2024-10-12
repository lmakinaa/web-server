#include "CGI.hpp"

void CGI::scriptToHtml(const char* cgiPath, const char* argv[], std::string& buff)
{
    int fds[2];
    pipe(fds);

    int pid = fork();
    if (!pid) {
        dup2(fds[1], 1);
        if (execve(cgiPath, const_cast<char* const*>(argv), NULL) == -1) {
            if (M_DEBUG)
                perror("execve(2)");
            exit(1);
        }
    } else if (pid == -1) {
        if (M_DEBUG)
            perror("fork(2)");
    } else {
        close(fds[1]);
        if (waitpid(pid, NULL, 0) == -1) {
            if (M_DEBUG)
                perror("waitpid(2)");
        } else {
            char* cbuff = new char[BUFF_SIZE]();
            size_t count = 0;
            while (1) {
                int r = read(fds[0], cbuff, BUFF_SIZE);

                if (r <= 0) {
                    if (r == -1 && M_DEBUG)
                        perror("execve(2)");
                    break ;
                }

                count += r;
                if (count > buff.max_size())
                    break ;

                buff.append(cbuff, r);
                std::memset(cbuff, 0, r + 1);
                if (r < BUFF_SIZE)
                    break ;
            }
            delete[] cbuff;
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