#include "WebServ.hpp"


void WebServ::run()
{
	Server& s = servers[0];
	s.init();


	int kq = kqueue();
	if (kq == -1) {
		std::cerr << "Kqueue(2) failed\n";
	}

	struct kevent event_change;
	EV_SET(&event_change, s.getSocket(), EVFILT_READ, EV_ADD, 0, 0, 0);
	kevent(kq, &event_change, 1, 0, 0, 0);

	
	
	int toWait = 1;
	while (true)
	{
		struct kevent events[toWait];
		// (nevents < 1) && (nevents = 1);
		int nevents = kevent(kq, nullptr, 0, events, toWait, nullptr);
        if (nevents == -1) {
            std::cerr << "Kevent failed: " << strerror(errno) << std::endl;
            break;
        }

		for (int i = 0; i < nevents; i++) {
			struct kevent* curr = &events[i];

			if (curr->ident == (uintptr_t)s.getSocket()) {
				
				// Handling new Connection
				int cSock = accept(curr->ident, (sockaddr*)&s.m_sockAddress, &s.m_sockLen);
				toWait++;

				int flags = fcntl(cSock, F_GETFL);
				fcntl(cSock, F_SETFL, flags | O_NONBLOCK);
				
				EV_SET(&event_change, cSock, EVFILT_READ, EV_ADD, 0, 0, 0);
				if (kevent(kq, &event_change, 1, nullptr, 0, nullptr) == -1) {
                    std::cerr << "Kevent failed for new connection: " << strerror(errno) << std::endl;
                    close(cSock);
                    continue;
                }
				
				std::cout << "Connection accepted" << std::endl ;
			} else {

				// Handling existing connection
				char buf[1024];
				int r = recv(curr->ident, buf, 1023, 0);

				if (r <= 0) {
					EV_SET(&event_change, curr->ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					kevent(kq, &event_change, 1, NULL, 0, NULL);
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

				EV_SET(&event_change, curr->ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				kevent(kq, &event_change, 1, NULL, 0, NULL);
				close(curr->ident);
				toWait--;
			}

		}
	}

	close(s.getSocket());
	close(kq);
}