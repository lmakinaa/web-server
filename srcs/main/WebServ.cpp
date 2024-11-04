#include "WebServ.hpp"

static int checkAndOpen(HttpRequest* req)
{
    int body_fd;
    int fd;

    M_DEBUG && std::cerr << "\033[1;31m|" << req->uri << "|\033[0m" << std::endl;

    // custom error pages
    Directive *error_page = NULL;
    Location *location = NULL;

    std::map<std::string, Directive>::iterator eit = req->mainServ->directives.find("error_page");
    if ( eit != req->mainServ->directives.end())
        error_page = &(eit->second);

    //Replace %20 with " "
    size_t p = 0;
    while ((p = req->uri.find("%20", p)) != std::string::npos) {
        req->uri.replace(p, 3, " ");
        p += 1;
    }


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
    req->uri = _GET_DELETE(*req->mainServ, uri, req->method, &location); // this give the path of the file
    std::string extension = "";

    size_t pPos = req->uri.find_last_of(".");
    if (pPos != std::string::npos)
        extension = req->uri.substr(pPos);
    if (!strcmp(extension.c_str(), ".php") || !strcmp(extension.c_str(), ".py") || !strncmp(extension.c_str(), ".php?", 5) || !strncmp(extension.c_str(), ".py?", 4))
        req->IsCgi = true;
    if (req->IsCgi)
    {
        std::cerr << "chikaviww chikaviiw bum chaa chaaa   " << req->uri <<std::endl;
        req->uri += querystr;
        M_DEBUG && std::cerr << "\033[1;31m" << req->bodyFile << "\033[0m" << std::endl;
        if (req->bodyFile.empty())
            body_fd = 0;
        else {
            body_fd = open(req->bodyFile.c_str(), O_RDONLY);
            unlink(req->bodyFile.c_str());
        }
        if (body_fd < 0)
            throw ErrorStatus(503, "Error opening body file in checkAndOpen", error_page);
        fd = CGI::responseCGI(req, body_fd, location);
    }
    else {
        fd = open(req->uri.c_str(), O_RDONLY);
        if (fd < 0)
            throw (ErrorStatus(500, "open failed in checkAndOpen", error_page));
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
        // looking for the right server
        std::string host = req->getHeader("Host");

        if (host.back() == '\n')
            host.pop_back();
        if (host.back() == '\r')
            host.pop_back();

        if ((*req->s).size() == 1)
        {
            req->mainServ = &(*req->s)[0];
        }
        else
        {
            for (size_t i = 0; i < req->s->size(); i++)
            {
                if ((*req->s)[i].server_name == host)
                {
                    req->mainServ = &(*req->s)[i];
                    break ;
                }
            }
        }

        if (req->mainServ == NULL)
        {
            req->mainServ = &(*req->s)[0];

        }
       

        // custom error pages
        Directive *error_page = NULL;

        std::map<std::string, Directive>::iterator eit = req->mainServ->directives.find("error_page");
        if ( eit != req->mainServ->directives.end())
            error_page = &(eit->second);

        
        KQueue::removeWatch(current->ident, EVFILT_READ);


        int fd = checkAndOpen(req);
        KQueue::setFdNonBlock(fd);
        
        M_DEBUG && std::cerr << "Request parsed and passed to execution\n" ;

        if (req->IsCgi) {
            t_eventData* evData = new t_eventData("cgi ready", new HttpResponse(current->ident, fd, req));
            if (KQueue::watchChildExited(req->cgiPid, evData) == -1)
                throw ErrorStatus(503, "WatchState at handleExistedConnection(1)", error_page);
            KQueue::startedCgis[evData] = std::time(NULL);
        } else {
            t_eventData* evData = new t_eventData("send response", new HttpResponse(current->ident, fd, req));
            if (KQueue::watchState(current->ident, evData, EVFILT_WRITE) == -1)
                throw ErrorStatus(503, "watchState in handleExistedConnection(2)", error_page);
        }

        std::map<t_eventData*, time_t>::iterator it = KQueue::connectedClients.find((t_eventData*)current->udata);
        KQueue::connectedClients.erase(it);

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

    KQueue::waitForClientToSend(clientSock, serverEvData->s);
	return 0;
}


void WebServ::cgiSwitchToSending(struct kevent* current)
{

    t_eventData* evData = (t_eventData*) current->udata;

    std::map<t_eventData*, time_t>::iterator it = KQueue::startedCgis.find(evData);
    KQueue::startedCgis.erase(it);

    int clientSocket = ((HttpResponse*)evData->resData)->clientSocket;

    int s;
    waitpid(current->ident, &s, WNOHANG);

    std::cerr << "status: " << s << '\n';

    if (s != 0)
        throw ErrorStatus(clientSocket, 500, "in cgiSwitchToSending: cgi failed");

    evData->type = "send response";
    // tal hna 3ad yt7ato lheaders dyal response

    if (KQueue::watchState(clientSocket, evData, EVFILT_WRITE) == -1)
        throw ErrorStatus(clientSocket, 503, "watchState in cgiSwitchToSending");
}

void WebServ::sendResponse(struct kevent* current)
{
    t_eventData* evData = (t_eventData*) current->udata;
    HttpResponse *res = (HttpResponse*)evData->resData;

    res->sendingResponse(current->data);

    if (res->ended) {
        KQueue::removeWatch(res->clientSocket, EVFILT_WRITE);
        // Salat response kolchi daz mzyan
        
        if (res->connectionClose)
            close(res->clientSocket);
        else {
            KQueue::waitForClientToSend(res->clientSocket, res->s);
            std::cerr << "entered here\n";
            m_watchedStates++;
        }

        m_watchedStates--;
        delete evData;
    }
}

void WebServ::loop()
{
    while (true)
    {
        struct kevent events[m_watchedStates];
        int nevents = KQueue::getEvents(events, m_watchedStates, m_watchedStates);

        for (int i = 0; i < nevents; i++) {
            try {
                if (!std::strcmp((static_cast<t_eventData*>(events[i].udata))->type, "server socket")) {

                    if (handleNewConnection(&events[i]) == -1)
                        continue ;
                    m_watchedStates++;
                    M_DEBUG && std::cerr << "Connection accepted" << std::endl ;
                }
                else if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "client socket")) {
                    M_DEBUG && std::cerr << "receiving\n";
                    handleExistedConnection(&events[i]);
                }
                else if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "cgi ready")) {
                    cgiSwitchToSending(&events[i]);
                }
                else if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "send response")) {
                    sendResponse(&events[i]);                    
                }

            } catch(ErrorStatus& e) {
                if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "client socket")
                    || !std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "send response"))
                    e.clientSock = events[i].ident;
            
                e.sendError();
                if (events[i].udata) {
                    if (std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "cgi ready")) {
                        std::map<t_eventData*, time_t>::iterator it = KQueue::connectedClients.find((t_eventData*)events[i].udata);
                        KQueue::connectedClients.erase(it);
                    }
                    delete (t_eventData*)events[i].udata;
                }
                m_watchedStates--;
                // connection will be closed automatically by getting out of the scope of the catch
            }
            catch(SuccessStatus& e) {
                if (!std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "client socket")
                    || !std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "send response"))
                    e.clientSock = events[i].ident;

                e.sendSuccess();
                if (events[i].udata) {
                    if (std::strcmp(static_cast<t_eventData*>(events[i].udata)->type, "cgi ready")) {
                        std::map<t_eventData*, time_t>::iterator it = KQueue::connectedClients.find((t_eventData*)events[i].udata);
                        KQueue::connectedClients.erase(it);
                    }
                    delete (t_eventData*)events[i].udata;
                }
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
