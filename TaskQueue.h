#ifndef IMITATER_TASKQUEUE_H
#define IMITATER_TASKQUEUE_H

#include "Logger.h"
#include "Uncopyable.h"

#include <deque>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace imitater
{

    template <class T>
    class TaskQueue : uncopyable
    {
    private:
        int _maxSize;
        std::deque<T> _taskQueue;
        std::mutex _queueMtx;
        std::condition_variable _queueEmptyCon;
    public:
        TaskQueue(int size)
        {
            _maxSize = size;
        }

        ~TaskQueue()
        {
        }

        typedef std::shared_ptr<TaskQueue> TaskQueuePtr;

        void pushTask(T task)
        {
            std::unique_lock lock(_queueMtx);
            if (isFull())
            {
                LOG_WARN << "Push task failed since task queue full.";
                return;
            }
            _taskQueue.push_back(std::move(task));
            _queueEmptyCon.notify_one();
        }

        T popTask()
        {
            std::unique_lock<std::mutex> lock(_queueMtx);
            while (isEmpty())
                _queueEmptyCon.wait(lock);
            T task = move(_taskQueue.front());
            _taskQueue.pop_front();
            return task;
        }

        void setMaxSize(int size)
        {
            std::unique_lock<std::mutex> lock(_queueMtx);
            _maxSize = size;
        }

        bool isEmpty() const
        {
            return _taskQueue.empty();
        }

        bool isFull() const
        {
            return _taskQueue.size() == _maxSize;
        }
    };
}

#endif