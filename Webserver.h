#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "Httpconn.h"
#include "Epoller.h"
#include "threadpool.h"
#include"Log.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Timer.h"
class Webserver
{
public:
    Webserver();
    ~Webserver();
    void Initsocket();
    void Start();
private:
    std::shared_ptr<Timer> times;
    std::shared_ptr<Epoller> epolls;
    std::map<int, Httpconn *> https;
    std::shared_ptr<ThreadPool> threadpool;
    Log* log;
    int listenfd;
    sockaddr_in address;
};
#endif
