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
    void registerEventor(Eventor::EventorPtr eventor);  // function provided by eventloop must be multi-thread.
    void updateEventor(Eventor::EventorPtr eventor);
    void unregisterEventor(Eventor::EventorPtr eventor);
    void execInLoop(FuncInLoop func);

    void unregisterEventorImidiate(Eventor::EventorPtr eventor);    // would not push task in loop; this is a Compromise Method for
                                                                    // close socket behavior. suggest call this in loop task.

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