#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <time.h>
#include <functional>
#include <unistd.h>
#include <unordered_map>
class Timer
{
    typedef std::chrono::system_clock::time_point timepoint;

private:
    class list
    {
    public:
        list *pre;
        list *next;
        timepoint time;
        std::function<void()> cb;
        list(std::function<void()> fcn = nullptr, list *p = nullptr, list *n = nullptr) : cb(fcn), pre(p), next(n)
        {
        }
    };

public:
    Timer(size_t timeout_=10000);
    ~Timer();
    void push(size_t index);
    void adjust(size_t index);
    void tick();
private:
    void delete_index(list *t);
    void delete_index(size_t index);
private:
    list *list_start;
    list *list_end;
    std::unordered_map<int, list *> search;
    size_t timeout;
};
#endif