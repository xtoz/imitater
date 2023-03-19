#ifndef IMITATER_THREADLOOP_H
#define IMITATER_THREADLOOP_H

#include "TaskQueue.h"
#include <functional>
#include <thread>
#include <memory>

namespace imitater
{
class ThreadLoop
{
public:
    typedef std::function<void(void)> Task;
    typedef std::shared_ptr<ThreadLoop> ThreadLoopPtr;
    typedef ThreadLoopPtr TaskThreadPtr;
    typedef std::thread::id Tid;
    typedef std::function<void(Tid)> EndCallback;
    enum State {Waiting, Working, End};

    static TaskThreadPtr createTaskThread(TaskQueue<Task>::TaskQueuePtr queue); // by using static func to create private construct obj,
                                                                                // can do init work which cannot be exec in construct.

    ThreadLoop(TaskQueue<Task>::TaskQueuePtr queue);    // in theory, de/constructor should be private, but shared_ptr requires public.
    ~ThreadLoop();                                      // this can be a short of std::shared_ptr.

    void setEndCallback(EndCallback cb);
    State state() const;
    Tid tid() const;

private:
    std::thread _thread;
    State _state;
    TaskQueue<Task>::TaskQueuePtr _taskQueue;
    EndCallback _endCallback;

    void setThread(std::thread & trd);
    void run();
};

}

#endif