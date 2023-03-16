#include "TaskQueue.h"
#include "Logger.h"

using namespace imitater;
using namespace std;

template<class T>
TaskQueue<T>::TaskQueue(int size)
{
    _maxSize = size;
}

template<class T>
TaskQueue<T>::~TaskQueue()
{

}

template<class T>
void TaskQueue<T>::pushTask(T task)
{
    unique_lock lock(_queueMtx);
    if(isFull())
    {
        LOG_WARN << "Push task failed since task queue full.";
        return;
    }
    _taskQueue.emplace(move(task));
    _queueEmptyCon.notify_one();
}

template<class T>
T TaskQueue<T>::popTask()
{
    unique_lock<mutex> lock(_queueMtx)
    while(isEmpty())
        _queueEmptyCon.wait(lock);
    T task = move(_taskQueue.front());
    _taskQueue.pop_front();
    return task;
}

template<class T>
void TaskQueue<T>::setMaxSize(int size)
{
    unique_lock<mutex> lock(_queueMtx)
    _maxSize=size;
}

template<class T>
bool TaskQueue<T>::isEmpty() const
{
    return _taskQueue.empty();
}

template<class T>
bool TaskQueue<T>::isFull() const
{
    return _taskQueue.size() == _maxSize();
}