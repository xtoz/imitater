#ifndef IMITATER_THREADLOOP_H
#define IMITATER_THREADLOOP_H

#include "TaskQueue.h"
#include "Uncopyable.h"

#include <functional>
#include <thread>
#include <memory>
#include <mutex>

namespace imitater
{
class ThreadLoop : uncopyable
{
public:
    typedef std::function<void(void)> Task;
    typedef std::shared_ptr<ThreadLoop> ThreadLoopPtr;
    typedef ThreadLoopPtr TaskThreadPtr;
    typedef std::thread::id Tid;
    typedef std::function<void(Tid)> EndCallback;
    enum State {Waiting, Working, End};

    static TaskThreadPtr createTaskThread(TaskQueue<Task>::TaskQueuePtr queue, EndCallback cb); // by using static func to create 
                                                                                // private construct obj, can do init work which
                                                                                // cannot be exec in construct.

    ThreadLoop(TaskQueue<Task>::TaskQueuePtr queue);    // in theory, de/constructor should be private, but shared_ptr
    ~ThreadLoop();                                      // requires public. this can be a short of std::shared_ptr.

    
    State state() const;
    Tid tid() const;
    void join();

private:
    std::thread _thread;
    State _state;
    TaskQueue<Task>::TaskQueuePtr _taskQueue;
    EndCallback _endCallback;
    std::mutex _initMtx;
    std::condition_variable _initCv;

    void setEndCallback(EndCallback cb);
    void setThread(std::thread & trd);
    void run();
};

}

#endif