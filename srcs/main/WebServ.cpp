#include "WebServ.hpp"

int WebServ::handleNewConnection(struct kevent* current)
{
    t_eventData *serverEvData = (t_eventData*)current->udata;
    t_sockData* tmp = (t_sockData*)serverEvData->data;

	int clientSock = accept(current->ident, (sockaddr*)(tmp->sockAddress), tmp->sockLen);
    if (clientSock == -1)
        perror("accept(2)");


    KQueue::setFdNonBlock(clientSock);

    HttpRequest *req = new HttpRequest();
    t_eventData *clientEvData = new t_eventData("client socket", req);

	KQueue::watchState(clientSock, clientEvData, EVFILT_READ);
	return 0;
}

int WebServ::handleExistedConnection(struct kevent* current)
{
    // Read and Parse Request
    HttpRequest* req = (HttpRequest*) ((t_eventData*)current->udata)->data;

    req->readRequest(current->ident);

    if(req->isDone == true) {
        KQueue::removeWatch(current->ident, EVFILT_READ);

        delete req;
        delete (t_eventData*)current->udata;

        // Open something (file or pipe) and pass it to response
        int fd = open("testimg.jpg", O_RDWR); // I will open index.html for exemple
        if (fd <= 0)
            perror("open(2)");

        KQueue::setFdNonBlock(fd);
		
        M_DEBUG && std::cerr << "Request parsed and passed to execution\n" ;
        KQueue::watchState(fd, new t_eventData("response ready", (void*)new HttpResponse(current->ident, fd)), EVFILT_READ);
        return 0;
    }
	return 0;
}

void WebServ::switchToSending(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    evData->type = "send response";

    KQueue::removeWatch(current->ident, EVFILT_READ);
    KQueue::watchState(current->ident, evData, EVFILT_WRITE);
}

void WebServ::sendResponse(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    HttpResponse *res = (HttpResponse*)evData->data;

    res->sendingResponse();

    if (res->ended) {
        KQueue::removeWatch(current->ident, EVFILT_WRITE);
        delete res;
        delete evData;
    }
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

            if (!std::strcmp((static_cast<t_eventData*>(events[i].udata))->type, "server socket")) {

                handleNewConnection(&events[i]);
                m_openedSockets++;
				std::cout << "Connection accepted" << std::endl ;
            }
            else if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "client socket")) {
				handleExistedConnection(&events[i]);
            }
            else if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "response ready")) {
                switchToSending(&events[i]);
            }
            else if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "send response")) {
                sendResponse(&events[i]);
            }

        }
        // if (waitpid(-1, NULL, WNOHANG) == -1)
        //     std::perror("waitpid(2)");
    }
}