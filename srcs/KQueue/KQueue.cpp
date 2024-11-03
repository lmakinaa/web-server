#include "KQueue.hpp"

int KQueue::m_fd = -1;
struct kevent KQueue::m_keventBuff;
struct timespec KQueue::m_timout = {TIMEOUT_SEC, 0};
std::map<t_eventData*, std::time_t> KQueue::connectedClients;

void KQueue::setFdNonBlock(int fd)
{
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

int KQueue::createKq()
{
    m_fd = kqueue();
    if (m_fd == -1)
       throw std::runtime_error(std::string("kqueue(2): ") + strerror(errno));
    return m_fd;
}

void KQueue::closeKq()
{
    close(m_fd);
}

// If it fails it will close the fd
// Check the return only when using for the server socket, otherwise you can ignore it
int KQueue::watchState(int fd, t_eventData* evData, int type)
{
    EV_SET(&KQueue::m_keventBuff, fd, type, EV_ADD, 0, 0, (void*)evData);
	if (kevent(m_fd, &KQueue::m_keventBuff, 1, 0, 0, 0) == -1) {
        if (M_DEBUG)
            perror("kevent(2)");
        // close(fd);
        return -1;
    }
    return 0;
}

void KQueue::removeWatch(int fd, int type)
{
    EV_SET(&m_keventBuff, fd, type, EV_DELETE, 0, 0, NULL);
    if (kevent(m_fd, &m_keventBuff, 1, NULL, 0, NULL) == -1 && M_DEBUG)
        perror("kevent(2) in removeWatch");
    // close(fd);
}

// It will returns 0 if kevent fails (To help ignore it silently)
// It will print error if M_DEBUG is set to 1
int KQueue::getEvents(struct kevent* buffArray, int size)
{
    int res = kevent(m_fd, NULL, 0, buffArray, size, &m_timout);
    if (res == -1) {
        if (M_DEBUG)
            perror("kevent(2)");
        return 0;
    } else if (res == 0) {
        time_t now = time(NULL);
        std::map<t_eventData*, std::time_t>::iterator e = connectedClients.end();
        for (std::map<t_eventData*, std::time_t>::iterator i = connectedClients.begin(); i != e; ) {
            if (now - i->second >= TIMEOUT_SEC) {
                removeWatch(i->first->reqData->clientSocket, EVFILT_READ);
                close(i->first->reqData->clientSocket);
                delete i->first;
                i = connectedClients.erase(i);
            }
            else
                i++;
        }
        return 0;
    }

    return res;
}

void KQueue::waitForClientToSend(int clientSock, Server* s)
{
    HttpRequest *req = new HttpRequest();
    req->s = s;
    req->clientSocket = clientSock;

    t_eventData *clientEvData = new t_eventData("client socket", req);

	EV_SET(&KQueue::m_keventBuff, clientSock, EVFILT_READ, EV_ADD, 0, 0, (void*)clientEvData);
	if (kevent(m_fd, &KQueue::m_keventBuff, 1, 0, 0, NULL) == -1) {
        if (M_DEBUG)
            perror("kevent(2) in waitForClientToSend");
        delete clientEvData;
        close(clientSock);
    }
    connectedClients[clientEvData] = time(NULL);
}

int KQueue::watchChildExited(pid_t pid, t_eventData* evData)
{
    EV_SET(&m_keventBuff, pid, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, 0, (void *)evData);

    if (kevent(m_fd, &m_keventBuff, 1, NULL, 0, NULL) == -1) {
        if (M_DEBUG)
            perror("kevent(2) in watchChildExited");
        return -1;
    }
    return 0;
}
