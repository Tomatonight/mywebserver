#ifndef HTTPCONN_H
#define HTTPCONN_H
#include "Buffer.h"
#include"Log.h"
#include <map>
#include <sys/stat.h>
#include <regex>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/uio.h>
class Httpconn
{
public:
   
    Httpconn(int sockfd_);
    ~Httpconn();
    int read();
    bool send();
    void process();
    static void init();
       bool Keep_alive();
private:
    void clear();
    bool parse();
    void parse_request(std::string &s);
    void parse_line(std::string &s);
    void parse_body();
    bool equal_line(std::string &line, const std::string &s);
 
    void fill_send();
    bool find_url();
private:
    Buffer buffer_receive;
    Buffer buffer_send;
    std::map<std::string, std::string> map_line;
    std::string method;
    std::string version;
    std::string url;
    struct stat st;
    char *file;
    iovec iov[2];
    int status;
    static std::string pre_url;
    static std::map<int, std::string> code_status;
    int sockfd;
};
#endif