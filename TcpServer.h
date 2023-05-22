#ifndef IMITATER_TCPSERVER_H
#define IMITATER_TCPSERVER_H

// #include "EventLoopPool.h"
#include "Acceptor.h"
#include "TcpConnection.h"

namespace imitater
{
class TcpServer : uncopyable
{
public:
    enum ThreadMode {USER_THREAD, SINGLE_THREAD, MULTI_THREAD};
    TcpServer(ThreadMode threadMode=USER_THREAD);

    void start();
    void setNewConnectionCallback(Acceptor::NewConnectionCallback cb);

private:
    EventLoop::EventLoopPtr _loopPtr;
    Acceptor::AcceptorPtr _acceptorPtr;
    ThreadMode _threadMode;
};
}

#endif