#include "ThreadPool.h"

using namespace imitater;
using namespace std;

ThreadPool::ThreadPool(int coreSize, int maxSize):
_coreSize(coreSize),
_maxSize(maxSize),
_taskQueue(make_shared<TaskQueue<ThreadLoop::Task>>(_maxSize))
{
    _threads.reserve(_maxSize);
    // TODO:consider create core thread when needed, for example, no waiting state thread and new task come.
}

ThreadPool::~ThreadPool()
{
    // TODO:any resource should be release?
}

void ThreadPool::createCoreThreads()
{
    while(_threads.size() <= _coreSize)
        createThread();
}

void ThreadPool::pushTask(ThreadLoop::Task task)
{
    {
        // TODO: consider into overload mode when exist too much task:
        // consider: if (task_num > threshold && thread_num < overload) then create some thread;
        // if (task_num < threshold && thread_num > core) then delete some thread;
        // this seems like lazy eliminate of memcached (see my note in obsidian).
    }
    _taskQueue->pushTask(task);
}

void ThreadPool::createThread()
{
    ThreadLoop::ThreadLoopPtr loop =  ThreadLoop::createTaskThread(_taskQueue, bind(&ThreadPool::threadEndCallback,shared_from_this(),placeholders::_1));
    _threads.emplace_back(loop);
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