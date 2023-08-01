#include "Timer.h"
Timer::Timer(size_t timeout_) : timeout(timeout_), list_start(nullptr), list_end(nullptr)
{
}
Timer::~Timer()
{
    for (auto &i : search)
    {
        delete i.second;
    }
}

void Timer::push(size_t index)
{
    if (search.find(index) == search.end())
    {
        list *new_list = new list([=]()
                                  { std::cout<<"Timer close: "<<index<<std::endl;
                                    close(index); });

        search[index] = new_list;
    }
    list *add = search[index];
    add->time = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
    if (list_start == nullptr)
    {
        list_start = add;
        list_end = add;
    }
    else if (list_start == list_end)
    {
        list_end = add;
        list_start->next = add;
        add->pre = list_start;
    }
    else
    {
        list_end->next = add;
        add->pre = list_end;
        list_end = add;
    }
}
void Timer::adjust(size_t index)
{

    list *t = search[index];
    t->time = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
    if (t == list_start && t == list_end)
    {
        return;
    }
    else if (t == list_start && t != list_end)
    {
        list_start = list_start->next;
        list_start->pre = nullptr;
        list_end->next = t;
        t->pre = list_end;
        list_end = t;
        t->next = nullptr;
    }
    else if (list_end == t)
    {
        return;
    }
    else
    {
        t->pre->next = t->next;
        t->next->pre = t->pre;
        t->pre = list_end;
        list_end->next = t;
        t->next = nullptr;
        list_end = t;
    }
}
void Timer::tick()
{

    while (list_start && (list_start->time) < std::chrono::system_clock::now())
    {

        delete_index(list_start);
        std::cout << "tick" << std::endl;
    }
}

void Timer::delete_index(list *t)
{
    t->cb();
     std::cout << "delete one" << std::endl;
    if (list_end == list_start)
    {
        list_end = nullptr;
        list_start = nullptr;
    }
    else if (t == list_start)
    {
        list_start = list_start->next;
        list_start->pre = nullptr;
    }
    else if (t == list_end)
    {
        list_end = list_end->pre;
        list_end->next = nullptr;
    }
    else
    {
        t->pre->next = t->next;
        t->next->pre = t->pre;
    }

}
void Timer::delete_index(size_t index)
{
    list *t = search[index];
    delete_index(t);
}