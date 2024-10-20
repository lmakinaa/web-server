#include "KQueue.hpp"

int KQueue::m_fd = -1;
struct kevent KQueue::m_keventBuff;

void KQueue::setFdNonBlock(int fd)
{
    int oldFlags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oldFlags | O_NONBLOCK);
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
int KQueue::watchFd(int fd, t_eventData* evData)
{
    EV_SET(&KQueue::m_keventBuff, fd, EVFILT_READ, EV_ADD, 0, 0, (void*)evData);
	if (kevent(m_fd, &KQueue::m_keventBuff, 1, 0, 0, 0) == -1) {
        if (M_DEBUG)
            perror("kevent(2)");
        close(fd);
        return -1;
    }
    return 0;
}

void KQueue::removeFd(int fd)
{
    EV_SET(&m_keventBuff, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(m_fd, &m_keventBuff, 1, NULL, 0, NULL) == -1 && M_DEBUG)
        perror("kevent(2) in removeFd");
    // close(fd);
}

// It will returns 0 if kevent fails (To help ignore it silently)
// It will print error if M_DEBUG is set to 1
int KQueue::getEvents(struct kevent* buffArray, int size)
{
    int res = kevent(m_fd, NULL, 0, buffArray, size, NULL);
    if (res == -1) {
        if (M_DEBUG)
            perror("kevent(2)");
        return 0;
    }
    return res;
}
