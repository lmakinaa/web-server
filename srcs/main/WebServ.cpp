#include "WebServ.hpp"

WebServ::WebServ()
    : m_cEventData("client socket", NULL)
    , m_openedSockets (0)
{
}

// returns -1 if kevent failed
int WebServ::handleNewConnection(struct kevent* current)
{
    t_eventData *evData = (t_eventData*)current->udata;
    t_sockData* tmp = (t_sockData*)evData->data;

	int clientSock = accept(current->ident, (sockaddr*)(tmp->sockAddress), tmp->sockLen);
    if (clientSock == -1)
        perror("accept(2)");

    m_openedSockets++;

	// int flags = fcntl(clientSock, F_GETFL);
	// fcntl(clientSock, F_SETFL, flags | O_NONBLOCK);
    KQueue::setFdNonBlock(clientSock);

	KQueue::watchFd(clientSock, &m_cEventData);
	return 0;
}

int WebServ::handleExistedConnection(struct kevent* current)
{
	char buf[1024];
	int r = recv(current->ident, buf, 1023, 0);

	if (r <= 0) {
		KQueue::removeFd(current->ident);
        close(current->ident);
        m_openedSockets--;
		return -1;
	}

	buf[r] = '\0';
	std::cout << buf << std::flush;

	
    // Parse Request
    // ...
    KQueue::removeFd(current->ident);
    // close(current->ident); // dont close we need to send data to it
	// m_openedSockets--;

    // Pass to CGI
    const char* argv[3] = {"php-cgi", "/Users/ijaija/web-server/srcs/CGI/test.php", NULL};
    std::string postBody ("var1=5454&var2=test&path=sds");
    CGI::runScript(
        POST,
        "/Users/ijaija/web-server/www/server-cgis/php-cgi",
        argv,
        postBody, current->ident);
    // m_openedSockets++;

	return 0;
}

void WebServ::sendResponse(struct kevent* current)
{
    std::string content;

    CGI::readOutput(current->ident, content);
    m_openedSockets--;

	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;

    if (send((long)current->udata, response.c_str(), response.size(), 0) == -1) {
		(M_DEBUG) && std::cerr << "error while sending response: " << strerror(errno) << '\n' ;
	}

    close((long)current->udata);
}

void WebServ::run()
{
    KQueue::createKq();

    for (size_t i = 0; i < servers.size(); i++) {
        servers[i].init();

        EV_SET(&KQueue::m_keventBuff, servers[i].getSocket(), EVFILT_READ, EV_ADD, 0, 0, (void*)&(servers[i].m_sEventData));
        if (kevent(KQueue::getFd(), &KQueue::m_keventBuff, 1, 0, 0, 0) == -1) {
            if (M_DEBUG)
                perror("kevent(2)");
            close(servers[i].getSocket());
            throw std::runtime_error("There was an error while adding server socket to kqueue");
        }
        m_openedSockets++;
    }

    while (true)
    {
        struct kevent events[m_openedSockets];
        int nevents = KQueue::getEvents(events, m_openedSockets);

        for (int i = 0; i < nevents; i++) {

            if ((long)events[i].udata > OPEN_MAX && !std::strcmp((static_cast<t_eventData*>(events[i].udata))->type, "server socket")) {

                handleNewConnection(&events[i]);
				std::cout << "Connection accepted" << std::endl ;
            }
            else if ((long)events[i].udata > OPEN_MAX && !std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "client socket")) {
				handleExistedConnection(&events[i]);
				std::cout << "Request parsed" << std::endl ;
            }
            else {
                sendResponse(&events[i]);
				std::cout << "Response sent" << std::endl ;
            }
        }
        if (waitpid(-1, NULL, WNOHANG) == -1)
            std::perror("waitpid(2)");
    }
}
