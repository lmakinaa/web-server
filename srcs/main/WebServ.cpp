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
        int fd;
        int body_fd;

        KQueue::removeWatch(current->ident, EVFILT_READ);

        // Open something (file or pipe) and pass it to response
        std::cout << "\033[1;31m|" << req->uri.c_str() << "|\033[0m" << std::endl;

        // We should handle directories and root here 
        if (req->uri == "/" || req->uri == "")
            req->uri = "/index.html";
        std::string extension = req->uri.substr(req->uri.find_last_of("."));
        if (extension == ".php" || extension == ".py")
        {
            std::cout << "\033[1;31m" << req->bodyFile << "\033[0m" << std::endl;
            if (req->bodyFile.empty())
                body_fd = 0;
            else
                body_fd = open(req->bodyFile.c_str(), O_RDONLY);
            if (body_fd < 0)
                perror("Error opening body file");
            fd = CGI::responseCGI(req, body_fd);
           
        }
        else
            fd = open(req->uri.c_str()+1, O_RDONLY);
        if (fd <= 0)
            perror("open(2)");

        KQueue::setFdNonBlock(fd);
		
        M_DEBUG && std::cerr << "Request parsed and passed to execution\n" ;
        KQueue::watchState(fd, new t_eventData("response ready", (void*)new HttpResponse(current->ident, fd, WhatContentType(req->uri))), EVFILT_READ);
        
        delete req;
        delete (t_eventData*)current->udata;

        return 0;
    }
	return 0;
}

void WebServ::switchToSending(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    evData->type = "send response";

    std::cerr << ((HttpResponse*)evData->data)->clientSocket;

    KQueue::removeWatch(current->ident, EVFILT_READ);
    KQueue::watchState(((HttpResponse*)evData->data)->clientSocket, evData, EVFILT_WRITE);
}

void WebServ::sendResponse(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    HttpResponse *res = (HttpResponse*)evData->data;

    res->sendingResponse(current->data);

    if (res->ended) {
        KQueue::removeWatch(res->clientSocket, EVFILT_WRITE);
        close(res->clientSocket);
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
