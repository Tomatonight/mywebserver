#include "Log.h"
Log *Log::Instance()
{
    static Log log;
    return &log;
};
Log::Log() : flag(false), cnt(0), line_cnt(0), fd_file(0)
{

    int ret = mkdir("log", 0777);
    assert(ret != -1 || (errno & EEXIST));
    create_newfile();
    thread_write = std::move(std::thread([&]()
                                         {
            std::unique_lock<std::mutex> l(m);
           
        while (!stop)
        {
            while(buffer[flag].size()<=0&&!stop)cv.wait_for(l, std::chrono::seconds(3));
            flag = !flag;
            if (line_cnt > 1000)
            {
                create_newfile();
            }
            
                buffer[!flag].send_to_fd(fd_file);

            buffer[!flag].clear();
         
        } }));
}
Log::~Log()
{
    stop = true;
    thread_write.join();
}
void Log::create_newfile()
{
    if (fd_file != 0)
        close(fd_file);
    auto time_now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(time_now);
    struct tm *t = localtime(&time);
    bzero(file_name, sizeof(file_name));
    sprintf(file_name, "log/%d-%02d-%02d_%d.log", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, cnt);
    fd_file = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0777);
    assert(fd_file != -1);
}
void Log::write(std::string &&s)
{
    write(s.data(), s.size());
}
void Log::write(std::string &s)
{
    write(s.data(), s.size());
}
void Log::write(char *s, size_t len)
{
    std::lock_guard<std::mutex> l(thread_mutex);
    line_cnt++;
    buffer[flag].write_to_buffer(s, len);
    buffer[flag].write_to_buffer("\n", 1);
    if (buffer[flag].size() > 1024)
    {
        cv.notify_one();
    }
}
void Log::write(char *s)
{
    write(s, strlen(s));
}
bool Log::stop = false;