#include "ThreadPool.h"

using namespace imitater;

void ThreadPool::setMaxThreadNum(int max)
{

}

void ThreadPool::setOverloadNum(int overload)
{

}

void ThreadPool::pushTask(Task task)
{
    _taskQueue.pushTask(task);
}

void ThreadPool::createThread()
{
    _threads.emplace_back([this]{
        while (true) {
            auto task = _taskQueue.popTask();
            if (!task) break;
            task();
            }
        }
    );
}

void ThreadPool::deleteThread()
{
    _taskQueue.pushTask(nullptr);   // if a thread receive a nullptr function, it will end.
}