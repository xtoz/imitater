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
_trdPool(make_shared<ThreadPool>(_maxLoopNum, _maxLoopNum))
{
    _curIndex = 0;
    _trdPool->createCoreThreads();
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
        // TODO: now eventlooppool exist forever, so 'this' donnot have problem.
        _trdPool->pushTask(bind(&EventLoopPool::createAndExecLoop, this));
        _loopArrCond.wait(lock);
    }

    _curIndex = _curIndex % _loopVec.size();
    LOG_NORMAL << "Current return loop : "<< to_string(_curIndex).c_str();
    EventLoop::EventLoopPtr retPtr = _loopVec[_curIndex++];
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