#include "ThreadPool.h"

using namespace imitater;
using namespace std;

ThreadPool::ThreadPool(int coreSize, int maxSize):
_coreSize(coreSize),
_maxSize(maxSize),
_taskQueue(make_shared<TaskQueue<ThreadLoop::Task>>(_maxSize))
{
    _threads.reserve(_maxSize);
    // TODO:consider create thread when needed, for example, no waiting state thread.
    for (int i = 0; i < coreSize; ++i)
        createThread();
}

ThreadPool::~ThreadPool()
{
    // TODO:any resource should be release?
}

void ThreadPool::pushTask(ThreadLoop::Task task)
{
    {
        // TODO: overload mode when exist task too much
        // consider: if (task_num > threshold && thread_num < overload) then create some thread;
        // if (task_num < threshold && thread_num > core) then delete some thread;
    }
    _taskQueue->pushTask(task);
}

void ThreadPool::createThread()
{
    ThreadLoop::createTaskThread(_taskQueue);

}

void ThreadPool::deleteThread()
{
    _taskQueue->pushTask(nullptr);   // if a thread receive a nullptr function, it will end.
}

void ThreadPool::threadEndCallback(ThreadLoop::Tid tid)
{
    unique_lock<mutex> lock(_trdVecMtx);
    for(int i=0;i<_threads.size();++i)
    {
        if(_threads[i]->tid() == tid)
        {
            _threads.erase(_threads.begin()+i);
            break;
        }
    }
}