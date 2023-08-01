#ifndef BUFFER_H
#define BUFFER_H
#include <memory.h>
#include <assert.h>
#include <unistd.h>
#include <string>
class Buffer
{

public:
    Buffer(size_t capacity_= 1024);
    ~Buffer();
    inline void clear();
    inline size_t size();
    inline char *ptr_start();
    inline char *ptr_end();
    inline size_t residue();
    std::string advance_until();
    char operator[](size_t index);
    void write_to_buffer(char *s, size_t len);
    void write_to_buffer(const char *s);
    void write_to_buffer(char *s);
    void write_to_buffer(std::string &s);
    size_t write_to_buffer_from_fd(int fd);
    void send_to_fd(int fd);
    std::string read_from_buffer_all();
    std::string read_from_buffer(size_t len);
private:
    void expand(size_t new_capacity);
    void ensure(size_t size_);
private:
    char *buffer;
    size_t capacity;
    size_t index_start;
    size_t index_end;
};
#endif