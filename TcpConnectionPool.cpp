#include "TcpConnectionPool.h"
#include "Logger.h"

using namespace imitater;
using namespace std;

const char TcpConnectionPool::_maxStep = 4;
const char TcpConnectionPool::_maxDlt = 100;
TcpConnectionPool* TcpConnectionPool::instance = new TcpConnectionPool();

bool TcpConnRc::unused()
{
    unique_lock<mutex> lock(_mtxUse);
    return (1==_data.use_count());
}

TcpConnectionPool::TcpConnectionPool()
{
    _head = nullptr;
    _current = _head;
    _step = 0;
    _dlt = 0;
}


TcpConnectionPool::~TcpConnectionPool()
{

}

TcpConnectionPool* TcpConnectionPool::getInstance()
{
    if(nullptr==instance)
        instance = new TcpConnectionPool();
    return instance;
}


void TcpConnectionPool::release()
{
    if(nullptr!=instance) {
        delete instance;
        instance = nullptr;
    }
}
    

TcpConnection::TcpConnectionPtr TcpConnectionPool::getTcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, TcpConnection::InitedCallback cb)
{
    unique_lock<mutex> lock(_mtxConn);

    auto temp = _head;
    cout << "chain: ";
    while(temp != nullptr) {
        cout << temp->_data.use_count() << " ";
        temp = temp->_next;
    }
    cout << endl;

    // if head is null, create head;
    if(_head == nullptr) {
        _head = new TcpConnRc();
        // _head->_data = make_shared<TcpConnection>(socket, loop);
        _head->_data = TcpConnection::createTcpConnection(socket, loop, cb);
        _current = _head;
        LOG_NORMAL << "Create new TcpConnection pool head.";
        return _current->_data;
    }

    TcpConnRc* rc = _current;
    TcpConnRc* prev = nullptr;

    // find in chain list
    while(rc != nullptr)
    {
        if(rc->unused()) {
            LOG_NORMAL << "Find a unused TcpConnection in pool.";
            TcpConnection::reusedTcpConnection(rc->_data, socket, loop, cb);
            // rc->_data(socket, loop);
            // rc->_data = make_shared<TcpConnection>(socket, loop);
            return rc->_data;
        }
        else {
            prev = rc;
            rc = rc->_next;
            if(++_step >= _maxStep) {
                LOG_NORMAL << "Pool current move forward.";
                _step = 0;
                _current = rc;
            }
        }
    }

    // not find in chain list, create new one;
    LOG_NORMAL << "Create new TcpConnection in pool.";
    rc = new TcpConnRc();
    // rc->_data = make_shared<TcpConnection>(socket, loop);
    rc->_data = TcpConnection::createTcpConnection(socket, loop, cb);
    prev->_next = rc;

    if (++_dlt >= _maxDlt) {
        LOG_NORMAL << "Reset pool current to head.";
        _dlt = 0;
        _current = _head;
    }

    return rc->_data;
}