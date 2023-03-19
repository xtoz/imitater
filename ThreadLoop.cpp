#include "ThreadLoop.h"

using namespace imitater;
using namespace std;

ThreadLoop::ThreadLoopPtr ThreadLoop::createTaskThread(TaskQueue<ThreadLoop::Task>::TaskQueuePtr queue)
{
    ThreadLoop::ThreadLoopPtr _trdloop = make_shared<ThreadLoop>(queue);
    thread _thread(std::bind(&ThreadLoop::run, _trdloop));
    _trdloop->setThread(_thread);
    return _trdloop;
}

ThreadLoop::ThreadLoop(TaskQueue<Task>::TaskQueuePtr queue)
{
    _endCallback = nullptr;
    _taskQueue = queue;
    _state = Waiting;
}

ThreadLoop::~ThreadLoop()
{
    // TODO:any resource should be release?
}

void ThreadLoop::setThread(std::thread& trd)
{
    _thread = move(trd);
}

void ThreadLoop::run()
{
    while(true)
    {
        auto task = _taskQueue->popTask();
        _state = Working;
        if (!task)
            break;
        else
            task();
        _state = Waiting;
    }
    _state=End;
    if(_endCallback)
        _endCallback(tid());
}

void ThreadLoop::setEndCallback(EndCallback cb)
{
    _endCallback = cb;
}

ThreadLoop::State ThreadLoop::state() const
{
    return _state;
}

ThreadLoop::Tid ThreadLoop::tid() const
{
    return _thread.get_id();
}