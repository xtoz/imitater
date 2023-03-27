#ifndef IMITATER_THREADPOOL_H
#define IMITATER_THREADPOOL_H

#include "TaskQueue.h"
#include "ThreadLoop.h"
#include "Uncopyable.h"

#include <functional>
#include <thread>
#include <vector>

namespace imitater
{
class ThreadPool : uncopyable, public std::enable_shared_from_this<ThreadPool>
{
public:
    explicit ThreadPool(int coreSize, int maxSize);
    ~ThreadPool();

    typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;

    void createCoreThreads();
    void pushTask(ThreadLoop::Task task);

private:
    int _coreSize;
    int _maxSize;

    TaskQueue<ThreadLoop::Task>::TaskQueuePtr _taskQueue;
    std::vector<ThreadLoop::TaskThreadPtr> _threads;
    std::mutex _trdVecMtx;

    void createThread();
    void deleteThread();
    void threadEndCallback(ThreadLoop::Tid tid);
};
}

#endif