#ifndef IMITATER_ACCEPTOR_H
#define IMITATER_ACCEPTOR_H

#include "TcpConnection.h"

namespace imitater
{
class Acceptor : public std::enable_shared_from_this<Acceptor>
{
public:
    explicit Acceptor(EventLoop::EventLoopPtr loop);
    ~Acceptor();

    typedef std::shared_ptr<Acceptor> AcceptorPtr;
    typedef std::function<void(TcpConnection::TcpConnectionPtr)> NewConnectionCallback;

    void setNewConnectionCallback(NewConnectionCallback cb);
    void listen();

private:
    const Socket::SocketPtr _socketPtr;
    const Eventor::EventorPtr _eventorPtr;
    const EventLoop::EventLoopPtr _loopPtr;

    NewConnectionCallback _newConnectoinCallback;

    void hadnleRead();
    void listenInLoop();
    TcpConnection::TcpConnectionPtr acceptNewConnection();
};
}

#endif