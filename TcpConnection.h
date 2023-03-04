#ifndef IMITATER_TCPCONNECTION_H
#define IMITATER_TCPCONNECTION_H

#include "EventLoop.h"
#include "Buffer.h"

namespace imitater
{
class TcpConnection : public std::enable_shared_from_this<TcpConnection>    // enable_shared seems must be public
{
public:
    explicit TcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop);
    ~TcpConnection();

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(void)> ReadCallback;
    typedef std::function<void(void)> WriteCallback;
    typedef std::function<void(void)> CloseCallback;

    void setReadCallback(ReadCallback cb);
    void setWriteCallback(WriteCallback cb);
    void setCloseCallback(CloseCallback cb);

    void setRead(int on);
    void setWrite(int on);

    void read(void* data, int len);
    void write(void* data, int len);
    void close();

private:
    const Socket::SocketPtr _socketPtr;
    const Eventor::EventorPtr _eventorPtr;
    const EventLoop::EventLoopPtr _loopPtr;
    ReadCallback _readCallback;
    WriteCallback _writeCallback;
    CloseCallback _closeCallback;
    const int _SockBufferSize;   // init 1024
    char* _sockReadBuffer;  // consider [auto expand size when full] buffer to abort these variate;
    char* _sockWriteBuffer;
    Buffer _readBuffer;
    Buffer _writeBuffer;

    void handleRead();
    void handlWrite();
    void handleClose();

    void writeInLoop();
};
}

#endif