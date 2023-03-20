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
    explicit TcpServer();

    void start();
    void setNewConnectionCallback(Acceptor::NewConnectionCallback cb);

private:
    EventLoop::EventLoopPtr _loopPtr;
    Acceptor::AcceptorPtr _acceptorPtr;
};
}

#endif