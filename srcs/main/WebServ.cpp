#include "WebServ.hpp"

// returns -1 if kevent failed
int WebServ::handleNewConnection(Server& server, struct kevent* current)
{
	int clientSock = accept(current->ident, (sockaddr*)&server.m_sockAddress, &server.m_sockLen);
	m_openedSockets++;

	int flags = fcntl(clientSock, F_GETFL);
	fcntl(clientSock, F_SETFL, flags | O_NONBLOCK);

	KQueue::watchSocket(clientSock);
	return 0;
}

int WebServ::handleOldConnection(struct kevent* current)
{
	char buf[1024];
	int r = recv(current->ident, buf, 1023, 0);

	if (r <= 0) {
		KQueue::removeSocket(current->ident);
		return -1;
	}

	buf[r] = '\0';
	std::cout << buf << std::endl;

	std::string content = "<h1>Hi!</h1>\n";
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;

	if (send(current->ident, response.c_str(), response.size(), 0) == -1) {
		std::cerr << "error while sending response\n";
	}

	KQueue::removeSocket(current->ident);
	m_openedSockets--;

	return 0;
}



void WebServ::run()
{
	Server& s = servers[0];
	s.init();

	KQueue::createKq();

	if (KQueue::watchSocket(s.getSocket()) == -1)
		throw std::runtime_error("There was an error while adding server socket to kqueue");

	m_openedSockets = 1;
	while (true)
	{
		struct kevent events[m_openedSockets];

		int nevents = KQueue::getEvents(events, m_openedSockets);

		for (int i = 0; i < nevents; i++) {

			if (events[i].ident == (uintptr_t)s.getSocket()) {

				handleNewConnection(s, &events[i]);
				(M_DEBUG) && std::cout << "Connection accepted" << std::endl ;
				
			} else {
				handleOldConnection(&events[i]);
			}

		}
	}

	KQueue::closeKq();
}