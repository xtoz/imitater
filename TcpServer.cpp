#include "TcpServer.h"
#include "EventLoopPool.h"

using namespace imitater;
using namespace std;

TcpServer::TcpServer(ThreadMode threadMode):
_threadMode(threadMode)
// TODO:init action move to function start
// _loopPtr(EventLoopPool::getInstance()->getNextLoop()),
// _acceptorPtr(make_shared<Acceptor>(_loopPtr))
{
    switch(_threadMode) {
        case USER_THREAD:
            _loopPtr = make_shared<EventLoop>();
            _acceptorPtr = make_shared<Acceptor>(_loopPtr);
            break;
        case SINGLE_THREAD:
            break;
        case MULTI_THREAD:
            break;
        default:
            LOG_ERROR << "Incorrect threadMode";
    }
}

void TcpServer::start()
{
    _acceptorPtr->listen();
    switch(_threadMode) {
        case USER_THREAD:
            _loopPtr->loop();
            break;
        default:
            break;
    }
}

void TcpServer::setNewConnectionCallback(Acceptor::NewConnectionCallback cb)
{
    _acceptorPtr->setNewConnectionCallback(cb);
}