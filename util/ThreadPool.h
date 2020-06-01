#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace mrpc
{

class ThreadPool final
{

public:
    ThreadPool();
    ~ThreadPool();
    ThreadPool(const ThreadPool& ) = delete;
    void operator= (const ThreadPool& ) = delete;
    
}
}


// A powerful threadpool implementation with future feature

#endif
