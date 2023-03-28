#ifndef IMITATER_TCPCONNECTION_H
#define IMITATER_TCPCONNECTION_H

#include "EventLoop.h"
#include "Buffer.h"
#include "Uncopyable.h"

namespace imitater
{
class TcpConnection : public uncopyable, public std::enable_shared_from_this<TcpConnection>    // enable_shared seems must be public
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(TcpConnectionPtr)> InitedCallback;

    explicit TcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop);
    ~TcpConnection();

    typedef std::function<void(void)> ReadCallback;
    typedef std::function<void(void)> WriteCallback;
    typedef std::function<void(void)> CloseCallback;

    enum TcpConnState {Undefined, Connected, Closing, Closed};

    void setReadCallback(ReadCallback cb);  // no consider about multi-thread, user should be careful. Why not consider? because we
    void setWriteCallback(WriteCallback cb);// even do not know when user object destroied (and here do not require smart ptr), so
    void setCloseCallback(CloseCallback cb);// consider multi-thread has no meaning.
                                            // Sugguest set callback null only when conn has closed, (close callback will exec
                                            // when conn closed, you can set callback null after it).
                                            // In theory, set callback form null to exist is safe.
    void setRead(int on);   // multi-thread considered
    void setWrite(int on);

    void read(void* data, int len); // multi-thread considered
    void write(void* data, int len);
    void close();
    int state() const;

    static TcpConnectionPtr createTcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, InitedCallback cb);
    static TcpConnectionPtr reusedTcpConnection(TcpConnectionPtr conn, Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, InitedCallback cb);

private:
    Socket::SocketPtr _socketPtr; // Acceptor pass this to conn, and will not share with other
    Eventor::EventorPtr _eventorPtr;  // will share this with eventloop.selector
    EventLoop::EventLoopPtr _loopPtr; // other share this with conn
    ReadCallback _readCallback;
    WriteCallback _writeCallback;
    CloseCallback _closeCallback;
    const int _SockBufferSize;   // init 1024
    char* _sockReadBuffer;  // consider [auto expand size when full] buffer to abort this;
    char* _sockWriteBuffer; // consider [pass pointer of _writeBuffur] to abort this;
    Buffer _readBuffer;
    Buffer _writeBuffer;
    TcpConnState _state;    // only use in loop func (include IO loop func and execInLoop func)

    void handleRead();
    void handlWrite();
    void handleClose();

    void updateEventsInLoop(int events);

    void writeInLoop();
    void closeInLoop();
    void initInLoop(InitedCallback cb);
    void reused(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, InitedCallback cb);
    void reusedInLoop(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, InitedCallback cb);
    void reusedInLoop2(InitedCallback cb);
};
}

#endif