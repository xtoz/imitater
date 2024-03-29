#include "EventLoopPool.h"

#define Max_Loop_Num 1

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
_maxLoopNum(Max_Loop_Num),
_trdPool(make_shared<ThreadPool>(_maxLoopNum, _maxLoopNum))
{
    _curIndex = 0;
    _curLoopNum = 0;
    _trdPool->createCoreThreads();
}

EventLoopPool::~EventLoopPool()
{
    // TODO:any resource should be release?
}

EventLoop::EventLoopPtr EventLoopPool::getNextLoop()
{
    unique_lock<mutex> lock(_loopArrMtx);
    if(_curLoopNum < _maxLoopNum)
    {
        _curLoopNum++;
        // now eventlooppool exist forever, so 'this' donnot have problem.
        _trdPool->pushTask(bind(&EventLoopPool::createAndExecLoop, this));
        _loopArrCond.wait(lock);
    }

    _curIndex = _curIndex % _curLoopNum;
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