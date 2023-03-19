#include "EventLoop.h"
#include "Logger.h"
#include <thread>

using namespace imitater;
using namespace std;

thread_local EventLoop* t_loopInCurThread = nullptr;

EventLoop::EventLoop():
_timeoutUS(1000*1000)
{
    _exit = true;
    if(nullptr == t_loopInCurThread)
        t_loopInCurThread = this;
    else
        LOG_ERROR << "Redundant EventLoop created.";
    LOG_NORMAL << "EventLoop construct.";
}

EventLoop::~EventLoop()
{
    LOG_NORMAL << "EventLoop deconstruct.";
    t_loopInCurThread = nullptr;
}

void EventLoop::loop()
{
    if(!checkThread())
        return;

    _exit = false;

    while(!_exit)
    {
        Selector::EventorPtrList activeList = _selector.select(_timeoutUS);
        
        for(auto && eventor : activeList)
            eventor->handleEvents();

        handleFuncInLoop();
    }

    string addr = reinterpret_cast<const char*>(this);
    addr += " loop end";
    LOG_NORMAL << addr.c_str();
}

bool EventLoop::isLooping() const
{
    return !_exit;
}

void EventLoop::stopLoop()
{
    if(!checkThread())
        return;
    _exit = true;
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
        string log = "Check Thread Failed, this event loop may be redundant: ";
        string addr = reinterpret_cast<const char*>(this);
        LOG_ERROR << log.c_str() << addr.c_str();
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