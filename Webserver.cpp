#include "Webserver.h"
#include <iostream>
Webserver::Webserver() : times(new Timer), epolls(new Epoller), threadpool(new ThreadPool), log(Log::Instance())
{
}
Webserver::~Webserver()
{
}
void Webserver::Initsocket()
{

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(1234);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    epolls->add_fd(listenfd, EPOLLIN);
    int optval = 1;
    assert(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != -1);
    bind(listenfd, (sockaddr *)&address, (socklen_t)sizeof(address));
    listen(listenfd,100);
    Httpconn::init();
    log->write("Initsocket: listenfd: " + std::to_string(listenfd));
}
void Webserver::Start()
{
    while (true)
    {
        int cnt = epolls->wait_event();
        epoll_event *events = epolls->get_event();
        for (int i = 0; i < cnt; i++)
        {
            int fd = events[i].data.fd;
            if (fd == listenfd)
            {

                sockaddr_in address_client;
                socklen_t size = sizeof(address_client);
                int fd_client = accept(listenfd, (sockaddr *)&address_client, &size);
             //  log->write("receive new fd: " + std::to_string(fd_client));
                if (fd_client <= 0)
                    continue;
                times->push(fd_client);
                epolls->add_fd(fd_client, EPOLLIN | EPOLLONESHOT | EPOLLET | EPOLLRDHUP);
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
              //  log->write("receive  close fd: " + std::to_string(fd));

                epolls->del_fd(fd);
            }
            else if (events[i].events & (EPOLLIN))
            {
               // log->write("receive epollin fd: " + std::to_string(fd));

                threadpool->add_task([&, fd]
                                     {
               if(https.find(fd)==https.end()) https[fd]=new Httpconn(fd);
               int len=https[fd]->read();
           //  log->write("receive len: "+std::to_string(len));
               if(len==-1)
               {
                    epolls->del_fd(fd);
               }
                else {
                    https[fd]->process();
                    epolls->mod_fd(fd,EPOLLOUT|EPOLLRDHUP|EPOLLHUP|EPOLLONESHOT);
                
                }; });
                times->adjust(fd);
            }
            else if (events[i].events & (EPOLLOUT))
            {

              //  log->write("eceive epollout fd: " + std::to_string(fd));
                threadpool->add_task([&, fd]
                                     {
               if( https[fd]->send())
                epolls->mod_fd(fd,EPOLLIN|EPOLLRDHUP|EPOLLHUP|EPOLLONESHOT);
                else epolls->mod_fd(fd,EPOLLOUT|EPOLLRDHUP|EPOLLHUP|EPOLLONESHOT); });
             //   log->write("finish send: " + std::to_string(fd));
                if (!https[fd]->Keep_alive())
                {
                    epolls->del_fd(fd);
                }
            }
        }
        times->tick();
    }
}
