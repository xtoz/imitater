#include "TcpServer.h"
#include "EventLoopPool.h"

using namespace imitater;
using namespace std;

TcpServer::TcpServer():
_loopPtr(EventLoopPool::getInstance()->getNextLoop()),
_acceptorPtr(make_shared<Acceptor>(_loopPtr))
{

}

void TcpServer::start()
{
    _acceptorPtr->listen();
}

void TcpServer::setNewConnectionCallback(Acceptor::NewConnectionCallback cb)
{
    _acceptorPtr->setNewConnectionCallback(cb);
}