#include "Buffer.h"
Buffer::Buffer(size_t capacity_) : capacity(capacity_), index_start(0), index_end(0)
{
    buffer = new char[capacity];
    assert(buffer != nullptr);
}
Buffer::~Buffer()
{
    delete[] buffer;
}
inline void Buffer::clear()
{
    index_start = 0;
    index_end = 0;
}
inline size_t Buffer::size()
{
    return index_end - index_start;
}
inline char *Buffer::ptr_start()
{
    return buffer + index_start;
}
inline char *Buffer::ptr_end()
{
    return buffer + index_end;
}
inline size_t Buffer::residue()
{
    return capacity - size();
}
std::string Buffer::advance_until()
{
    int l = index_start;
    while (size() > 0 && buffer[index_start] != '\n')
        index_start++;
    index_start++;
    if (l >= index_start - 2)
        return "";
    return std::string(buffer + l, buffer + index_start - 2);
}
char Buffer::operator[](size_t index)
{
    assert(index >= index_start && index < index_end);
    return *(buffer + index);
}
void Buffer::write_to_buffer(char *s, size_t len)
{

    ensure(len);
    memcpy(ptr_end(), s, len);
    index_end += len;
}
void Buffer::write_to_buffer(const char *s)
{
    write_to_buffer((char *)s, strlen(s));
}
void Buffer::write_to_buffer(char *s)
{
    write_to_buffer(s, strlen(s));
}
void Buffer::write_to_buffer(std::string &s)
{
    write_to_buffer(s.data(), s.size());
}
size_t Buffer::write_to_buffer_from_fd(int fd)
{
    
    while(true)
    {
    char temp[65535];
    iovec iov[2];
    iov[0].iov_base=buffer;
    iov[0].iov_len=capacity;
    iov[1].iov_base=temp;
    iov[1].iov_len=65535;
    int len=readv(fd,iov,2);
    if(len<0)
    {
        if(errno&EAGAIN)
        continue;
        return -1;
    }
    if(len==0)break;
    else{
        if(len>capacity)
        {
            index_end=capacity;
            write_to_buffer(temp,len-capacity);
        }
        else
        {
            index_end+=len;
        }
    }
    return len;
    }
    
}
void Buffer::send_to_fd(int fd)
{
    while (size() > 0)
    {
        size_t len = write(fd, ptr_start(), size());
        index_start += len;
    }
}
std::string Buffer::read_from_buffer_all()
{
    std::string re(ptr_start(), ptr_end());
    clear();
    return re;
}
std::string Buffer::read_from_buffer(size_t len)
{
    assert(len <= size());
    std::string re(ptr_start(), ptr_start() + len);
    index_start += len;
    return re;
}
void Buffer::expand(size_t new_capacity)
{
    if (new_capacity <= capacity)
        return;
    char *temp = new char[new_capacity];
    assert(temp != nullptr);
    memcpy(temp, buffer + index_start, size());
    index_end = size();
    index_start = 0;
    delete[] buffer;
    buffer = temp;
}
void Buffer::ensure(size_t size_)
{
    if (capacity - index_end < size_)
    {
        if (capacity - size() >= size_)
        {
            memcpy(buffer, buffer + index_start, size());
            index_end = size();
            index_start = 0;
        }
        else
        {
            size_t new_capacity = capacity * 2 > capacity + size_ ? capacity * 2 : capacity + size_;
            expand(new_capacity);
        }
    }
};