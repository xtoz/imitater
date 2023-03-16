#ifndef IMITATER_THREADPOOL_H
#define IMITATER_THREADPOOL_H

#include "TaskQueue.h"

#include <functional>
#include <thread>
#include <vector>

namespace imitater
{
class ThreadPool
{
public:
    typedef std::function<void(void)> Task;

    void setMaxThreadNum(int max);
    void setOverloadNum(int overload);

    void pushTask(Task task);

private:
    int _maxNum;
    int _overloadNum;

    TaskQueue<Task> _taskQueue;
    std::vector<std::thread> _threads;

    void createThread();
    void deleteThread();
};
}

#endif