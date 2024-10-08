#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <climits>
#include <fcntl.h>

int main()
{
	int serverSock = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSock == -1) {
		std::cerr << "error while using socket(2)\n";
		std::exit(1);
	}
	sockaddr_in x;
	std::memset(x.sin_zero, 0, 8 * sizeof(unsigned char));
	x.sin_port = htons(8080);
	x.sin_family = AF_INET;
	x.sin_addr.s_addr = INADDR_ANY;
	
	int opt = 1;
	if (setsockopt(serverSock, SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		perror("setsockopt");
		exit(1);
	} 
	
	int f = fcntl(serverSock, F_GETFL);
	fcntl(serverSock, F_SETFL, f | O_NONBLOCK);

	int s = bind(serverSock, (sockaddr*) &x, sizeof(x));
	if (s < 0) {
		std::cerr << "error while using bind(2)\n";
		std::exit(1);
	}

	if (listen(serverSock, SOMAXCONN) == -1) {
		std::cerr << "error while using bind(2)\n";
		std::exit(1);
	}
	socklen_t slen;
	slen = sizeof(x);

	int kq;
	if ((kq = kqueue()) == -1) {
		std::cerr << "error while using kqueue(2)\n";
		std::exit(1);
	}

	struct kevent change_event;
	EV_SET(&change_event, serverSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(kq, &change_event, 1, NULL, 0, 0);

struct kevent events;
int ix = 0;
while (true) {
    kevent(kq, NULL, 0, &events, 1, NULL);
	
	// for (int i = 0; i < n_events; i++) {
		struct kevent *curr = &events;

		if (curr->ident == (uintptr_t) serverSock) {
			int cfd = accept(serverSock, (struct sockaddr *) &x, &slen);

			f = fcntl(cfd, F_GETFL);
			fcntl(f, F_SETFL, f | O_NONBLOCK);

			EV_SET(&change_event, cfd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
			kevent(kq, &change_event, 1, NULL, 0, 0);

			std::cout << "Conn Accepted" << std::endl;
		} else {
			++ix;
			char buf[1024];
            int r = recv(curr->ident, buf, 1023, 0);
            if (r <= 0) {
                close(curr->ident);
                continue;
            }

            buf[r] = '\0';
            std::cout << buf << std::endl;
            std::string content = "<h1>Hi!</h1>\n";
            std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;

            if (send(curr->ident, response.c_str(), response.size(), 0) == -1) {
                std::cerr << "error while sending response\n";
            }

            EV_SET(&change_event, curr->ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(kq, &change_event, 1, NULL, 0, NULL);
            close(curr->ident);

		}

	// }

	std::cout << ix << std::endl;

}
	close(serverSock);
    close(kq);

	return 0;
}



            // char buf[1024];
            // int r = recv(curr.ident, buf, 1023, 0);
            // if (r <= 0) {
            //     close(curr.ident);
            //     continue;
            // }

            // buf[r] = '\0';
            // std::cout << buf << std::endl;
            // std::string content = "<h1>Hi!</h1>\n";
            // std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;

            // if (send(curr.ident, response.c_str(), response.size(), 0) == -1) {
            //     std::cerr << "error while sending response\n";
            // }

            // EV_SET(&event, curr.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            // kevent(kq, &event, 1, NULL, 0, NULL);
            // close(curr.ident);