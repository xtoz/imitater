#include "ThreadLoop.h"

using namespace imitater;
using namespace std;

ThreadLoop::ThreadLoopPtr ThreadLoop::createTaskThread(TaskQueue<ThreadLoop::Task>::TaskQueuePtr queue, ThreadLoop::EndCallback cb)
{
    ThreadLoop::ThreadLoopPtr _trdloop = make_shared<ThreadLoop>(queue);
    _trdloop->setEndCallback(cb);
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
    {
        unique_lock<mutex> lock(_initMtx);
        _thread = move(trd);
    }
    _initCv.notify_all();
}

void ThreadLoop::run()
{
    {
        unique_lock<mutex> lock(_initMtx);
        _initCv.wait_for(lock, std::chrono::microseconds(500));  // after init run, but init can be already finished, so wait for 500 micro
                                                                 // sec, if 500 micro sec over and init do not finished and the thread will
                                                                 // close, the thread will not release in pool's vector, however, this is
                                                                 // almost impossible, and even happen will not bring fatal error.
    }
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

void ThreadLoop::join()
{
    _thread.join();
}