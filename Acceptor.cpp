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
    _loopPtr->unregisterEventor(_eventorPtr);
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback cb)
{
    _newConnectoinCallback = cb;
}

void Acceptor::listen()
{
    _eventorPtr->setReadCallback(std::bind(&Acceptor::hadnleRead, shared_from_this()));
    _eventorPtr->setEvents(Eventor::EventRead);
    _socketPtr->listen();
}
    
void Acceptor::hadnleRead()
{
    TcpConnection::TcpConnectionPtr newConn = acceptNewConnection();
    
    if(_newConnectoinCallback)
        _newConnectoinCallback(newConn);
}

TcpConnection::TcpConnectionPtr Acceptor::acceptNewConnection()
{
    Socket::SocketPtr newSocket = _socketPtr->accept();
    TcpConnection::TcpConnectionPtr conn = make_shared<TcpConnection>(newSocket, _loopPtr);
    return conn;
}