#include "TcpConnection.h"

using namespace imitater;
using namespace std;

TcpConnection::TcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop):
_socketPtr(socket),
_eventorPtr(make_shared<Eventor>(_socketPtr)),
_loopPtr(loop),
_SockBufferSize(1024)
{
    _loopPtr->registerEventor(_eventorPtr);

    _sockReadBuffer = new char[_SockBufferSize];
    _sockWriteBuffer = new char[_SockBufferSize];
    
    _readCallback = nullptr;
    _writeCallback = nullptr;
    _closeCallback = nullptr;
}

TcpConnection::~TcpConnection()
{
    _loopPtr->unregisterEventor(_eventorPtr);

    delete[] _sockReadBuffer;
    delete[] _sockWriteBuffer;
}

void TcpConnection::setRead(int on)
{
    int events=_eventorPtr->events();
    if(on)
    {
        events &= Eventor::EventRead;
    }
    else
    {
        events &= ~Eventor::EventRead;
    }
    _eventorPtr->setEvents(events);
    _eventorPtr->setReadCallback(std::bind(&TcpConnection::handleRead, shared_from_this()));
}

void TcpConnection::setWrite(int on)
{
    int events=_eventorPtr->events();
    if(on)
    {
        events &= Eventor::EventWrite;
    }
    else
    {
        events &= ~Eventor::EventWrite;
    }
    _eventorPtr->setEvents(events);
    _eventorPtr->setWriteCallback(std::bind(&TcpConnection::handlWrite, shared_from_this()));
}

void TcpConnection::setReadCallback(ReadCallback cb)
{
    _readCallback = cb;
}

void TcpConnection::setWriteCallback(WriteCallback cb)
{
    _writeCallback = cb;
}

void TcpConnection::setCloseCallback(CloseCallback cb)
{
    _closeCallback = cb;
}

void TcpConnection::read(void* data, int len)
{
    _readBuffer.pickRead(data, len);
}

void TcpConnection::write(void* data, int len)
{
    _writeBuffer.write(data, len);
    _loopPtr->execInLoop(std::bind(&TcpConnection::writeInLoop, shared_from_this()));
}

void TcpConnection::close()
{
    _socketPtr->close();
}

void TcpConnection::handleRead()
{
    int size = _socketPtr->read(_sockReadBuffer, _SockBufferSize);
    if(!size)
    {
        handleClose();
    }
    else
    {
        _readBuffer.write(_sockReadBuffer, size);
        if(_readCallback)
            _readCallback();
    }
}

void TcpConnection::handlWrite()
{
    
}

void TcpConnection::handleClose()
{
    if(_closeCallback)
        _closeCallback();
}

void TcpConnection::writeInLoop()
{
    int size = _writeBuffer.endPos();
    int minSize = ::min(size, _SockBufferSize);
    _writeBuffer.pickRead(_sockWriteBuffer, minSize);
    _socketPtr->write(_sockWriteBuffer, minSize);   // TODO:no consider real write size
    if(_writeBuffer.endPos() > 0)
    {
        _loopPtr->execInLoop(std::bind(&TcpConnection::writeInLoop, shared_from_this()));
    }
}