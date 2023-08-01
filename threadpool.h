#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <mutex>

class ThreadPool
{
private:
    std::vector<std::thread> threadpool;
    std::queue<std::function<void()>> tasks;
    bool stop;
    std::mutex m;
    std::condition_variable cv;

public:
    ThreadPool(int max_thread_count= 10);
    ~ThreadPool();
    template <class F>
    void add_task(F function);
};
ThreadPool::ThreadPool(int max_thread_count )
{

    for (int i = 0; i < max_thread_count; i++)
    {
        std::function<void()> task = [&]()
        {
            while (!stop)
            {
                std::function<void()> temp;
                if (stop)
                    return;

                {
                    std::unique_lock<std::mutex> l(m);
                    while (tasks.empty())
                    {
                        cv.wait(l, [&]()
                                { return stop == true || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                    }
                    temp = tasks.front();
                    tasks.pop();
                }

                temp();
             
            }
        };

        threadpool.emplace_back(std::thread(task));
    }
}
template <class F>
void ThreadPool::add_task(F function)
{
    if (stop)
        return;
    std::lock_guard<std::mutex> l(m);
    {
        std::function<void()> f = function;
        tasks.push(f);
    }
    cv.notify_one();
}
ThreadPool::~ThreadPool()
{
    stop = true;
    cv.notify_all();
    for (std::thread &t : threadpool)
    {
        t.join();
    }
}
#endif
