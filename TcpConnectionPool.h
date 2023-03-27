#ifndef IMITATER_TCPCONNECTIONPOOL_H
#define IMITATER_TCPCONNECTIONPOOL_H

#include "TcpConnection.h"

#include <queue>

namespace imitater
{
class TcpConnRc : public uncopyable    // Tcp Connection Resource
{
friend class TcpConnectionPool;
private:
    TcpConnRc() {_next = nullptr;}
    ~TcpConnRc() {};
    TcpConnection::TcpConnectionPtr _data;
    TcpConnRc* _next;
    bool unused();
    std::mutex _mtxUse;
};

class TcpConnectionPool : public uncopyable
{
public:
    static TcpConnectionPool* getInstance();
    static void release();
    TcpConnection::TcpConnectionPtr getTcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop);

private:
    TcpConnectionPool();
    ~TcpConnectionPool();
    TcpConnRc* _head;
    TcpConnRc* _current;
    std::mutex _mtxConn;
    char _step;
    char _dlt; // dilatation
    static const char _maxStep;  // set _current +4 every _maxStep
    static const char _maxDlt;  // reset _current as _head every (create new tcp connection) _maxDlt times;
    static TcpConnectionPool* instance;
};
typedef TcpConnectionPool TcpConnPool;
}

#endif