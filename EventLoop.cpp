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
        {
            eventor->handleEvents();
        }

        funcInLoop();
    }
}

void EventLoop::registerEventor(Eventor::EventorPtr eventor)
{
    _selector.registerEventor(eventor);
}

void EventLoop::updateEventor(Eventor::EventorPtr eventor)
{
    _selector.updateEventor(eventor);
}

void EventLoop::unregisterEventor(Eventor::EventorPtr eventor)
{
    _selector.unregisterEventor(eventor);
}

void EventLoop::execInLoop(FuncInLoop func)
{
    funcInLoopList.emplace_back(func);
}

void EventLoop::funcInLoop()
{
    vector<FuncInLoop> doList;
    doList.swap(funcInLoopList);
    for(auto && func : doList)
    {
        func();
    }
}