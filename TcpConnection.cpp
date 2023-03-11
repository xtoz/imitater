#include "TcpConnection.h"

using namespace imitater;
using namespace std;

TcpConnection::TcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop) : _socketPtr(socket),
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

    if (!_socketPtr->isValid())
    {
        _state = Undefined;
        // TODO:error
    }
    else
    {
        _state = Connected;
    }
}

TcpConnection::~TcpConnection()
{
    closeInLoop();  // do not worry, sock does not share with other, there is no one have sock when exec descontruct func.
    _loopPtr->unregisterEventor(_eventorPtr); // do not worry, this is a shared_ptr, and conn in event callback is weak ptr.

    delete[] _sockReadBuffer;
    delete[] _sockWriteBuffer;
}

void TcpConnection::setRead(int on)
{
    int events = _eventorPtr->events();
    if (on)
        events &= Eventor::EventRead;
    else
        events &= ~Eventor::EventRead;

    _loopPtr->execInLoop(std::bind(&TcpConnection::updateEventsInLoop, shared_from_this(), events));
}

void TcpConnection::setWrite(int on)
{
    int events = _eventorPtr->events();
    if (on)
        events &= Eventor::EventWrite;
    else
        events &= ~Eventor::EventWrite;

    _loopPtr->execInLoop(std::bind(&TcpConnection::updateEventsInLoop, shared_from_this(), events));
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

void TcpConnection::read(void *data, int len)
{
    _readBuffer.pickRead(data, len);
}

void TcpConnection::write(void *data, int len)
{
    if (_state != Connected)
        return;
    _writeBuffer.write(data, len);
    _loopPtr->execInLoop(std::bind(&TcpConnection::writeInLoop, shared_from_this()));
}

void TcpConnection::close()
{
    _loopPtr->execInLoop(std::bind(&TcpConnection::closeInLoop, shared_from_this()));
}

int TcpConnection::state() const
{
    return _state;
}

void TcpConnection::handleRead()
{
    if (_state != Connected)
        return;
    int size = _socketPtr->read(_sockReadBuffer, _SockBufferSize);
    if (!size) // on windows sys, read event with no message means socket close
    {
        handleClose();
    }
    else
    {
        _readBuffer.write(_sockReadBuffer, size);
        if (_readCallback)
            _readCallback();
    }
}

void TcpConnection::handlWrite()
{
    if (_state != Connected)
        return;
}

void TcpConnection::handleClose()
{
    if (_state == Closed)
        return;
    _state = Closed;
    if (_closeCallback)
        _closeCallback();
}

void TcpConnection::updateEventsInLoop(int events)
{
    if (_state != Connected)
        return;
    _eventorPtr->setEvents(events);
    if (events & Eventor::EventRead)
    {
        function<void(void)> callback = [weakObj = weak_from_this()]()
        {
            if (auto obj = weakObj.lock())
                obj->handleRead();
        };
        _eventorPtr->setReadCallback(callback);
    }
    else
    {
        _eventorPtr->setReadCallback(nullptr);
    }
    if(events & Eventor::EventWrite)
    {
        function<void(void)> callback = [weakObj = weak_from_this()]()
        {
            if (auto obj = weakObj.lock())
                obj->handlWrite();
        };
        _eventorPtr->setWriteCallback(callback);
    }
    else
    {
        _eventorPtr->setWriteCallback(nullptr);
    }
}

void TcpConnection::writeInLoop()
{
    if (_state != Connected)
        return;
    int size = _writeBuffer.size();
    int minSize = min(size, _SockBufferSize);
    _writeBuffer.pickRead(_sockWriteBuffer, minSize);
    _socketPtr->write(_sockWriteBuffer, minSize); // TODO:no consider how much data real be written in socket write
    if (_writeBuffer.size() > 0)
        _loopPtr->execInLoop(std::bind(&TcpConnection::writeInLoop, shared_from_this()));
}

void TcpConnection::closeInLoop()
{
    handleClose();
    _socketPtr->close();
}