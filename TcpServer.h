#ifndef IMITATER_TCPSERVER_H
#define IMITATER_TCPSERVER_H

#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"

namespace imitater
{
class TcpServer
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