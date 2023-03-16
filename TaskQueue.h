#ifndef IMITATER_TASKQUEUE_H
#define IMITATER_TASKQUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace imitater
{

template<class T>
class TaskQueue
{
public:
    explicit TaskQueue(int size);
    ~TaskQueue();
    void pushTask(T task);
    T popTask();
    void setMaxSize(int size);
    bool isEmpty() const;
    bool isFull() const;

private:
    int _maxSize;
    std::deque<T> _taskQueue;
    std::mutex _queueMtx;
    std::condition_variable _queueEmptyCon;
};
}

#endif