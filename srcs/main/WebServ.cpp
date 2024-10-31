#include "WebServ.hpp"

static int checkAndOpen(HttpRequest* req)
{
    int body_fd;
    int fd;

    // Open something (file or pipe) and pass it to response
    M_DEBUG && std::cerr << "\033[1;31m|" << req->uri.c_str() << "|\033[0m" << std::endl;
    
    if (req->getHeader("Cookie") == "")
        req->headers["Set-Cookie"] = "SESSID=" + sessionIdGen(*req->s);
    else
        req->headers["Set-Cookie"] = "";


    // remove querystring to check existance of the file
    std::string uri = req->uri;
    std::string querystr = "";
    size_t pos = req->uri.find("?");
    if (pos != std::string::npos)
    {
        querystr = req->uri.substr(pos);
        uri = req->uri.substr(0, pos);
    }
    // Rachid gad throws f _GET_DELETE() wcleani ressourses gbal ma throwi
    // We should handle directories and root here
    req->uri = _GET_DELETE(*req->s, uri, req->method); // this give the path of the file
    std::string extension = "";

    size_t pPos = req->uri.find_last_of(".");
    if (pPos != std::string::npos)
        extension = req->uri.substr(pPos);

    if (!strcmp(extension.c_str(), ".php") || !strcmp(extension.c_str(), ".py") || !strncmp(extension.c_str(), ".php?", 5) || !strncmp(extension.c_str(), ".py?", 5))
    {
        req->uri += querystr;
        M_DEBUG && std::cerr << "\033[1;31m" << req->bodyFile << "\033[0m" << std::endl;
        if (req->bodyFile.empty())
            body_fd = 0;
        else
            body_fd = open(req->bodyFile.c_str(), O_RDONLY);
        if (body_fd < 0)
            throw ErrorStatus(503, "Error opening body file in checkAndOpen");

        fd = CGI::responseCGI(req, body_fd);
    }
    else {
        fd = open(req->uri.c_str(), O_RDONLY);
        if (fd < 0)
            throw (ErrorStatus(500, "open failed in checkAndOpen"));
    }

    return fd;
}

int WebServ::handleExistedConnection(struct kevent* current)
{
    // Read and Parse Request
    HttpRequest* req = (HttpRequest*) ((t_eventData*)current->udata)->reqData;

    M_DEBUG && std::cerr << "Reading request\n";
    req->readRequest(current->ident);

    if(req->isDone == true)
    {
        KQueue::removeWatch(current->ident, EVFILT_READ);

        int fd = checkAndOpen(req);
        KQueue::setFdNonBlock(fd);
        
        M_DEBUG && std::cerr << "Request parsed and passed to execution\n" ;

        t_eventData* evData = new t_eventData("response ready", new HttpResponse(current->ident, fd, WhatContentType(req->uri)));
        if (KQueue::watchState(fd, evData, EVFILT_READ) == -1)
            (delete evData, close(fd), throw ErrorStatus(503, "WatchState at handleExistedConnection"));

        delete (t_eventData*)current->udata;
        current->udata = NULL;

        return 0;
    }

	return 0;
}

int WebServ::handleNewConnection(struct kevent* current)
{
    t_eventData *serverEvData = (t_eventData*)current->udata;
    t_sockData* tmp = (t_sockData*)serverEvData->serverData;

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

	if (KQueue::watchState(clientSock, clientEvData, EVFILT_READ) == -1)
        delete clientEvData;
	return 0;
}


void WebServ::switchToSending(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    evData->type = "send response";

    int clientSocket = ((HttpResponse*)evData->resData)->clientSocket;

    KQueue::removeWatch(current->ident, EVFILT_READ);
    if (KQueue::watchState(clientSocket, evData, EVFILT_WRITE) == -1)
        throw ErrorStatus(clientSocket, 503, "watchState in switchToSending");
}

void WebServ::sendResponse(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    HttpResponse *res = (HttpResponse*)evData->resData;

    res->sendingResponse(current->data);

    if (res->ended) {
        KQueue::removeWatch(res->clientSocket, EVFILT_WRITE);
        close(res->clientSocket);
        delete evData;
        m_watchedStates--;
        // Salat response kolchi daz mzyan
    }
}

void WebServ::loop()
{
    while (true)
    {
        struct kevent events[m_watchedStates];
        int nevents = KQueue::getEvents(events, m_watchedStates);

        for (int i = 0; i < nevents; i++) {
            try {
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

            } catch(ErrorStatus& e) {
                if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "client socket")
                    || !std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "send response"))
                    e.clientSock = events[i].ident;
            
                e.sendError();
                if (events[i].udata)
                    delete (t_eventData*)events[i].udata;
                m_watchedStates--;
                // connection will be closed automatically by getting out of the scope of the catch
            }

        }
        waitpid(-1, NULL, WNOHANG); // This is for cleaning the cgi processes that terminated
    }
}

void WebServ::run()
{
    KQueue::createKq();

    try {
        for (size_t i = 0; i < servers.size(); i++) {
            servers[i].init();

            if (KQueue::watchState(servers[i].getSocket(), &(servers[i].m_sEventData), EVFILT_READ) == -1) // sockets are closed in destructor of servers
                throw std::runtime_error("There was an error while adding server socket to kqueue");
            m_watchedStates++;
        }
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    while (true) 
    {
        try {
            loop();
        } catch(std::exception& e) {
            M_DEBUG && std::cerr << e.what() << '\n';
        } catch(std::out_of_range& e) {
            M_DEBUG && std::cerr << e.what() << '\n';
        }
    }

    
}

// void WebServ::fdCollector(int fd)
// {
//     static std::vector<int> nonSocketFds;

//     if (fd == -2) {
//         for (std::vector<int>::iterator i = nonSocketFds.begin(); i < nonSocketFds.end(); i++) {
//             close(*i);
//         }
//         return ;
//     }
//     nonSocketFds.push_back(fd);
// }