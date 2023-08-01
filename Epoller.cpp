 #include"Epoller.h"
    Epoller::Epoller(int max_events_ ) : max_events(max_events_)
    {
        epollfd = epoll_create(30);
        events = new epoll_event[max_events];
    }
    Epoller::~Epoller()
    {
        delete[] events;
    }
    void Epoller::add_fd(int fd, size_t events)
    {
        epoll_event event;
        event.data.fd = fd;
        setnoblock(fd);
        event.events = events;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    }
    void Epoller::del_fd(int fd)
    {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
    }
    void Epoller::mod_fd(int fd, size_t events)
    {
        epoll_event event;
        event.data.fd = fd;
        event.events = events;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
    }
    int Epoller::wait_event(int timeoutMs )
    {
        return epoll_wait(epollfd, events, max_events, timeoutMs);
    }
    epoll_event* Epoller::get_event()
    {
        return events;
    }
    void Epoller::setnoblock(int socket)
    {
        int old = fcntl(socket, F_GETFL, 0);
        fcntl(socket, F_SETFL, old | O_NONBLOCK);
    }
