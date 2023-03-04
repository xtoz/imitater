#include "TcpServer.h"

using namespace imitater;
using namespace std;

TcpServer::TcpServer():
_loopPtr(make_shared<EventLoop>()),
_acceptorPtr(make_shared<Acceptor>(_loopPtr))
{

}

void TcpServer::start()
{
    _acceptorPtr->listen();
    _loopPtr->loop();
}

void TcpServer::setNewConnectionCallback(Acceptor::NewConnectionCallback cb)
{
    _acceptorPtr->setNewConnectionCallback(cb);
}