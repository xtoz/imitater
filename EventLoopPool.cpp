#include "EventLoopPool.h"

using namespace imitater;
using namespace std;

EventLoopPool* EventLoopPool::_eventLoopPool = new EventLoopPool();

EventLoopPool* EventLoopPool::getInstance()
{
    return _eventLoopPool;
}

void EventLoopPool::release()
{
    _eventLoopPool = nullptr;
}

EventLoopPool::EventLoopPool() :
_maxLoopNum(4),
_trdPool(_maxLoopNum, _maxLoopNum)
{
    _curIndex = 0;
}

EventLoopPool::~EventLoopPool()
{
    // TODO:any resource should be release?
}

EventLoop::EventLoopPtr EventLoopPool::getNextLoop()
{
    unique_lock<mutex> lock(_loopArrMtx);
    if(_loopVec.size() < _maxLoopNum)
    {
        // TODO: maybe should put weakptr into threadpool if oneday threadpool is not a normal member, since this can be dangerous.
        _trdPool.pushTask(bind(&EventLoopPool::createAndExecLoop, this));
        _loopArrCond.wait(lock);
    }

    LOG_NORMAL << "Current return loop : "<< to_string(_curIndex % _loopVec.size()).c_str();
    
    EventLoop::EventLoopPtr retPtr = _loopVec[_curIndex];
    _curIndex = ++_curIndex % _loopVec.size();
    return retPtr;
}

void EventLoopPool::createAndExecLoop()
{
    EventLoop::EventLoopPtr loop = make_shared<EventLoop>();
    {
        unique_lock<mutex> lock(_loopArrMtx);
        _loopVec.push_back(loop);
    }
    _loopArrCond.notify_all();
    loop->loop();
}