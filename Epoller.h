#ifndef EPOLLER_H
#define EPOLLER_H
#include <sys/epoll.h>
#include<fcntl.h>
class Epoller
{
public:
    Epoller(int max_events_ = 100);
    ~Epoller();
    void add_fd(int fd, size_t events);
    void del_fd(int fd);
    void mod_fd(int fd, size_t events);
    epoll_event* get_event();
    int wait_event(int timeoutMs = -1);
 
private:
    void setnoblock(int socket);
private:
    int max_events;
    epoll_event *events;
    int epollfd;
};
#endif