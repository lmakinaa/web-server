#include "WebServ.hpp"

int WebServ::handleNewConnection(struct kevent* current)
{
    t_eventData *serverEvData = (t_eventData*)current->udata;
    t_sockData* tmp = (t_sockData*)serverEvData->data;

	int clientSock = accept(current->ident, (sockaddr*)(tmp->sockAddress), tmp->sockLen);
    if (clientSock == -1) {
        if (M_DEBUG)
            perror("accept(2)");
        return -1;
    }

    KQueue::setFdNonBlock(clientSock);

    HttpRequest *req = new HttpRequest();
    req->s = serverEvData->s;
    
    t_eventData *clientEvData = new t_eventData("client socket", req);

	KQueue::watchState(clientSock, clientEvData, EVFILT_READ);
	return 0;
}

int WebServ::handleExistedConnection(struct kevent* current)
{
    // Read and Parse Request
    HttpRequest* req = (HttpRequest*) ((t_eventData*)current->udata)->data;

    M_DEBUG && std::cerr << "Reading request\n";
    req->readRequest(current->ident);

    if(req->isDone == true)
    {
        KQueue::removeWatch(current->ident, EVFILT_READ);

        int fd;
        int body_fd;

        // Open something (file or pipe) and pass it to response
        M_DEBUG && std::cerr << "\033[1;31m|" << req->uri.c_str() << "|\033[0m" << std::endl;

        // We should handle directories and root here 
        if (req->uri == "/" || req->uri == "")
            req->uri = "/index.html";
        std::string extension = req->uri.substr(req->uri.find_last_of("."));
        if (!strcmp(extension.c_str(), ".php") || !strcmp(extension.c_str(), ".py") || !strncmp(extension.c_str(), ".php?", 5) || !strncmp(extension.c_str(), ".py?", 5))
        {
            M_DEBUG && std::cerr << "\033[1;31m" << req->bodyFile << "\033[0m" << std::endl;
            if (req->bodyFile.empty())
                body_fd = 0;
            else
                body_fd = open(req->bodyFile.c_str(), O_RDONLY);
            if (body_fd < 0)
                if (M_DEBUG) { // should clean and throw
                    perror("Error opening body file");
                    close(current->ident);
                    return -1;
                }
            fd = CGI::responseCGI(req, body_fd);
           
        }
        else
            fd = open(req->uri.c_str()+1, O_RDONLY);
        if (fd < 0)
            if (M_DEBUG) { // should clean and throw
                perror("Error opening body file");
                close(current->ident);
                return -1;
            }

        KQueue::setFdNonBlock(fd);
		
        M_DEBUG && std::cerr << "Request parsed and passed to execution\n" ;
        if (KQueue::watchState(fd, new t_eventData("response ready", (void*)new HttpResponse(current->ident, fd, WhatContentType(req->uri))), EVFILT_READ) == -1)
            ; // Should throw
        
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

    KQueue::removeWatch(current->ident, EVFILT_READ);
    if (KQueue::watchState(((HttpResponse*)evData->data)->clientSocket, evData, EVFILT_WRITE) == -1)
        ; // Should throw
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

        m_watchedStates--;
    }
}

void WebServ::run()
{
    KQueue::createKq();

    for (size_t i = 0; i < servers.size(); i++) {
        servers[i].init();

        if (KQueue::watchState(servers[i].getSocket(), &(servers[i].m_sEventData), EVFILT_READ) == -1) // sockets are closed in destructor of servers
            throw std::runtime_error("There was an error while adding server socket to kqueue");
        m_watchedStates++;
    }

    while (true)
    {
        struct kevent events[m_watchedStates];
        int nevents = KQueue::getEvents(events, m_watchedStates);

        for (int i = 0; i < nevents; i++) {

            if (!std::strcmp((static_cast<t_eventData*>(events[i].udata))->type, "server socket")) {

                if (handleNewConnection(&events[i]) == -1)
                    continue ;
                m_watchedStates++;
				M_DEBUG && std::cerr << "Connection accepted" << std::endl ;
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

            // // Handle closed socket
            // if (events[i].flags & EV_EOF) {
            //     M_DEBUG && std::cerr << "Client socket closed" << std::endl;
            //     close(events[i].ident);
            //     continue;
            // }

        }
        waitpid(-1, NULL, WNOHANG); // This is for cleaning the cgi processes that terminated
    }
}
