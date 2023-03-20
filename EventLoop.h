#ifndef IMITATER_EVENTLOOP_H
#define IMITATER_EVENTLOOP_H

#include "Selector.h"
#include "Uncopyable.h"

#include <functional>
#include <mutex>


namespace imitater
{
class EventLoop : uncopyable
{
public:
    EventLoop();
    ~EventLoop();

    typedef std::shared_ptr<EventLoop> EventLoopPtr;
    typedef std::function<void(void)> FuncInLoop;

    void loop();    // must exec in the same thread where the loop created.
    bool isLooping() const;
    void stopLoop();
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

    bool checkThread();
    void handleFuncInLoop();
};
}

#endif