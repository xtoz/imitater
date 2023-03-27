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
        std::deque<T> _pQueue; // productor
        std::deque<T> _cQueue; // consumer
        std::mutex _mtxP;
        std::mutex _mtxC;
        std::condition_variable _cvP;
        std::condition_variable _cvC;

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
            std::unique_lock<std::mutex> lock(_mtxP);
            while (_pQueue.size() == _maxSize)
            {
                // wait for comsumer.
                LOG_WARN << "Push task failed since task queue full.";
                _cvP.wait(lock);
            }
            _pQueue.push_back(std::move(task));
            // notify consumer.
            _cvC.notify_all();
        }

        T popTask()
        {
            std::unique_lock<std::mutex> lock(_mtxC);
            while (_cQueue.size() == 0)
            {
                std::unique_lock<std::mutex> lockP(_mtxP);
                _cQueue.swap(_pQueue);
                // notify productor.
                _cvP.notify_all();
                if(_cQueue.size() == 0)  // now consumer queue and productor queue both zero, and productor has be notified, so can wait.
                    _cvC.wait(lockP);
            }
            T task = move(_cQueue.front());
            _cQueue.pop_front();
            return task;
        }

        void setMaxSize(int size)
        {
            _maxSize = size;
        }
    };
}

#endif