#include "EventLoop.h"

using namespace imitater;
using namespace std;

EventLoop::EventLoop():
_timeoutUS(100000)
{
    _exit = false;
}

EventLoop::~EventLoop()
{

}

void EventLoop::loop()
{
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
    _selector.unregisterEventor(eventor);
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