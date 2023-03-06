#ifndef IMITATER_EVENTLOOP_H
#define IMITATER_EVENTLOOP_H

#include "Selector.h"
#include <functional>
#include <mutex>


namespace imitater
{
class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    typedef std::shared_ptr<EventLoop> EventLoopPtr;
    typedef std::function<void(void)> FuncInLoop;

    void loop();
    void registerEventor(Eventor::EventorPtr eventor);
    void updateEventor(Eventor::EventorPtr eventor);
    void unregisterEventor(Eventor::EventorPtr eventor);
    void execInLoop(FuncInLoop func);

private:
    Selector _selector;
    bool _exit;

    const int _timeoutUS; // defult 10000 us
    std::vector<FuncInLoop> funcInLoopList;
    std::mutex _mutexFuncInLoop;

    void handleFuncInLoop();
};
}

#endif