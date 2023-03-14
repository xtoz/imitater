#include "EventLoop.h"
#include "Logger.h"
#include <thread>

using namespace imitater;
using namespace std;

thread_local EventLoop* t_loopInCurThread = nullptr;

EventLoop::EventLoop():
_timeoutUS(100000)
{
    _exit = false;
    if(nullptr == t_loopInCurThread)
        t_loopInCurThread = this;
    else
        LOG_ERROR << "Redundant EventLoop created.";
}

EventLoop::~EventLoop()
{
    t_loopInCurThread = nullptr;
}

void EventLoop::loop()
{
    if(!checkThread())
        return;

    while(!_exit)
    {
        Selector::EventorPtrList activeList = _selector.select(_timeoutUS);
        
        for(auto && eventor : activeList)
            eventor->handleEvents();

        handleFuncInLoop();
    }
}

void EventLoop::registerEventor(Eventor::EventorPtr eventor)
{
    execInLoop(std::bind(&Selector::registerEventor, &_selector, eventor));
}

void EventLoop::updateEventor(Eventor::EventorPtr eventor)
{
    execInLoop(std::bind(&Selector::updateEventor, &_selector, eventor));
}

void EventLoop::unregisterEventor(Eventor::EventorPtr eventor)
{
    execInLoop(std::bind(&Selector::unregisterEventor, &_selector, eventor));
}

void EventLoop::execInLoop(FuncInLoop func)
{
    lock_guard<mutex> lock(_mutexFuncInLoop);
    funcInLoopList.emplace_back(func);
}

void EventLoop::unregisterEventorImidiate(Eventor::EventorPtr eventor)
{
    if(!checkThread())
        return;
    _selector.unregisterEventor(eventor);
}

bool EventLoop::checkThread()
{
    if(this != t_loopInCurThread)
    {
        string log = "Check Thread Failed, this object may be redundant: ";
        log.append(to_string((int)this));
        LOG_ERROR << log.c_str();
        return false;
    }

    return true;
}

void EventLoop::handleFuncInLoop()
{
    vector<FuncInLoop> doList;
    {
        lock_guard lock(_mutexFuncInLoop);
        doList.swap(funcInLoopList);
    }
    for(auto && func : doList)
    {
        func();
    }
}