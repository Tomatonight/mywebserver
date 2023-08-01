#ifndef LOG_H
#define LOG_H
#include "Buffer.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <atomic>
#include <assert.h>

class Log
{
public:
    static Log *Instance();

private:
    Log();
    ~Log();
    void create_newfile();

public:
    inline void write(std::string &&s);
    inline void write(std::string &s);
    inline void write(char *s, size_t len);
    inline void write(char *s);
    static bool stop;
private:
    std::thread thread_write;
    std::atomic<bool> flag;
    std::condition_variable cv;
    std::mutex m;
    std::mutex thread_mutex;
    Buffer buffer[2];
    int fd_file;
    char file_name[100];
    int cnt;
    int line_cnt;
};
#endif