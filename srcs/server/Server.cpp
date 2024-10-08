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

#define PORT 1234
#define BUFFER_SIZE 4096


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
	
	int yes = 1;
	if (setsockopt(serverSock, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("setsockopt");
		exit(1);
	} 

	int s = bind(serverSock, (sockaddr*) &x, sizeof(x));
	if (s < 0) {
		std::cerr << "error while using bind(2)\n";
		std::exit(1);
	}

	if (listen(serverSock, 20) == -1) {
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
	struct kevent event;
	EV_SET(&event, serverSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, 0);
	int i = 0;
	while (true) {
		i++;
		std::cout<<i<<'\n';
		struct kevent event_list[32];
		int n_events = kevent(kq, NULL, 0, event_list, 32, NULL);

		for (int i = 0; i < n_events; i++) {
			if (static_cast<int>(event_list[i].ident) == serverSock) {
				int cfd;
				if ((cfd = accept(serverSock, (sockaddr*) &x, &slen)) == -1) {
					std::cerr << "error while using bind(2)\n";
					std::exit(1);
				}
				char buf[1024] = {0};
				recv(cfd, buf, 1024, 0);
				std::cout << buf << std::endl;
				std::string content = "<h1>Hi!</h1>\n";
				std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
				send(cfd, response.c_str(), response.size(), 0);
				close(cfd);
			}
		}

	}

	return 0;
}
