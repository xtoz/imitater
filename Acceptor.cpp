#include "Acceptor.h"

using namespace imitater;
using namespace std;

Acceptor::Acceptor(EventLoop::EventLoopPtr loop) :
_socketPtr(make_shared<Socket>()),
_eventorPtr(make_shared<Eventor>(_socketPtr)),
_loopPtr(loop)
{
    _loopPtr->registerEventor(_eventorPtr);
}

Acceptor::~Acceptor()
{
    _loopPtr->unregisterEventor(_eventorPtr);   // do not worry, this is a shared_ptr, and will do before socket close.
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback cb)
{
    _newConnectoinCallback = cb;
}

void Acceptor::listen()
{
    _loopPtr->execInLoop(std::bind(&Acceptor::listenInLoop, shared_from_this()));
}
    
void Acceptor::hadnleRead()
{
    TcpConnection::TcpConnectionPtr newConn = acceptNewConnection();
    
    if(_newConnectoinCallback)
        _newConnectoinCallback(newConn);
}

void Acceptor::listenInLoop()
{
    function<void(void)> callback = [weakObj = weak_from_this()]()
    {
        if (auto obj = weakObj.lock())
            obj->hadnleRead();
    };
    _eventorPtr->setReadCallback(callback);
    _eventorPtr->setEvents(Eventor::EventRead);
    _socketPtr->listen();
}

TcpConnection::TcpConnectionPtr Acceptor::acceptNewConnection()
{
    Socket::SocketPtr newSocket = _socketPtr->accept();
    TcpConnection::TcpConnectionPtr conn = make_shared<TcpConnection>(newSocket, _loopPtr);
    return conn;
}