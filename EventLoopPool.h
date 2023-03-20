#ifndef IMITATER_EVENTLOOPPOOL_H
#define IMITATER_EVENTLOOPPOOL_H

#include "EventLoop.h"
#include "ThreadPool.h"
#include "Uncopyable.h"

#include <array>

namespace imitater
{
class EventLoopPool : uncopyable
{
public:
    static EventLoopPool* getInstance();
    void release();

    EventLoop::EventLoopPtr getNextLoop();

private:
    static EventLoopPool* _eventLoopPool;
    int _curIndex;
    const int _maxLoopNum;
    std::vector<EventLoop::EventLoopPtr> _loopVec;
    std::mutex _loopArrMtx;
    std::condition_variable _loopArrCond;
    ThreadPool _trdPool;

    EventLoopPool();
    ~EventLoopPool();
    void createAndExecLoop();
};
}

#endif