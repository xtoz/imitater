#include "TcpConnection.h"
#include "Logger.h"

using namespace imitater;
using namespace std;

static constexpr TcpConnState UNDEFINED = TcpConnState::TCPCONN_UNDEFINED;
static constexpr TcpConnState WAITINIT = TcpConnState::TCPCONN_WAITINIT;
static constexpr TcpConnState CONNECTED = TcpConnState::TCPCONN_CONNECTED;
static constexpr TcpConnState CLOSED = TcpConnState::TCPCONN_CLOSED;

TcpConnection::TcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop) :
StateMachine<TcpConnState>(UNDEFINED),
_socketPtr(socket),
_eventorPtr(make_shared<Eventor>(_socketPtr)),
_loopPtr(loop),
_SockBufferSize(1024)
{
    if(!turnTo(WAITINIT)) {
        LOG_WARN << "Turn to WAITINIT failed.";
        return;
    }

    _sockReadBuffer = new char[_SockBufferSize];
    _sockWriteBuffer = new char[_SockBufferSize];

    _readCallback = nullptr;
    _writeCallback = nullptr;
    _closeCallback = nullptr;

    LOG_NORMAL << "A tcpconnection constructed.";
}

TcpConnection::~TcpConnection()
{
    _loopPtr->unregisterEventor(_eventorPtr); // do not worry, this is a shared_ptr, and conn in event callback is weak ptr.

    delete[] _sockReadBuffer;
    delete[] _sockWriteBuffer;

    LOG_NORMAL << "A tcpconnection deconstructed.";
}

void TcpConnection::setRead(int on)
{
    int events = _eventorPtr->events();
    if (on)
        events |= Eventor::EventRead;
    else
        events &= ~Eventor::EventRead;

    // TODO: in theory, bind func should with weakptr, not sharedptr. but shared ptr in loop will be release at last, so maybe safe.
    _loopPtr->execInLoop(std::bind(&TcpConnection::updateEventsInLoop, shared_from_this(), events));
}

void TcpConnection::setWrite(int on)
{
    int events = _eventorPtr->events();
    if (on)
        events |= Eventor::EventWrite;
    else
        events &= ~Eventor::EventWrite;

    // TODO: in theory, bind func should with weakptr, not sharedptr. but shared ptr in loop will be release at last, so maybe safe.
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
    _writeBuffer.write(data, len);
    // TODO: in theory, bind func should with weakptr, not sharedptr. but shared ptr in loop will be release at last, so maybe safe.
    _loopPtr->execInLoop(std::bind(&TcpConnection::writeInLoop, shared_from_this()));
}

void TcpConnection::close()
{
    // TODO: in theory, bind func should with weakptr, not sharedptr. but shared ptr in loop will be release at last, so maybe safe.
    _loopPtr->execInLoop(std::bind(&TcpConnection::closeInLoop, shared_from_this()));
}

void TcpConnection::reused(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, InitedCallback cb)
{
    _loopPtr->execInLoop(std::bind(&TcpConnection::reusedInLoop, shared_from_this(), socket, loop, cb));
}

TcpConnection::TcpConnectionPtr TcpConnection::createTcpConnection(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, TcpConnection::InitedCallback cb)
{
    TcpConnection::TcpConnectionPtr newConn = make_shared<TcpConnection>(socket, loop);
    newConn->_loopPtr->execInLoop(std::bind(&TcpConnection::initInLoop, newConn, cb));
    return newConn;
}

TcpConnection::TcpConnectionPtr TcpConnection::reusedTcpConnection(TcpConnectionPtr conn, Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, TcpConnection::InitedCallback cb)
{
    conn->reused(socket, loop, cb);
    return conn;
}

void TcpConnection::handleRead()
{
    if(!checkState(CONNECTED))
        return;
    int size = _socketPtr->read(_sockReadBuffer, _SockBufferSize);
    if (size <= 0) // on windows sys, read event with size <= 0 means socket close
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
    if(!checkState(CONNECTED))
        return;
}

void TcpConnection::handleClose()
{
    if(!turnTo(CLOSED)) {
        LOG_WARN << "Turn to CLOSED failed.";
        return;
    }
    
    _socketPtr->close();
    _loopPtr->unregisterEventorImidiate(_eventorPtr);   // compromise method for close socket.
    if (_closeCallback)
        _closeCallback();
}

void TcpConnection::updateEventsInLoop(int events)
{
    if(!checkState(CONNECTED))
        return;
    _eventorPtr->setEvents(events);
    // compare with shared_ptr in loop, callback ptr will be hold forever, so callback ptr must be weak_ptr;
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
    if(!checkState(CONNECTED))
        return;
    int minSize = min(_writeBuffer.size(), _SockBufferSize);
    _writeBuffer.pickRead(_sockWriteBuffer, minSize);
    _socketPtr->write(_sockWriteBuffer, minSize); // TODO:no consider how much data real be written in socket write
    if (_writeBuffer.size() > 0)
        // TODO: in theory, bind func should with weakptr, not sharedptr. but shared ptr in loop will be release at last, so maybe safe.
        _loopPtr->execInLoop(std::bind(&TcpConnection::writeInLoop, shared_from_this()));
}

void TcpConnection::closeInLoop()
{
    if(!checkState(CONNECTED))
        return;
    handleClose();
}

void TcpConnection::initInLoop(InitedCallback cb)
{
    if(!turnTo(CONNECTED)) {
        LOG_WARN << "Turn to CONNECTED failed.";
        return;
    }

    _loopPtr->registerEventorImidiate(_eventorPtr);

    if(cb)
        cb(shared_from_this());
}

void TcpConnection::reusedInLoop(Socket::SocketPtr socket, EventLoop::EventLoopPtr loop, InitedCallback cb)
{
    if(!turnTo(WAITINIT)) {
        LOG_WARN << "Turn to WAITINIT failed.";
        return;
    }

    // release resource
    _loopPtr->unregisterEventorImidiate(_eventorPtr); // TODO: before unregister finish, user should not operate tcpConn, but now cannot
                                                      // promise. becase return tcpConnPtr and reusedInLoop are in different threads.
                                                      // Before register finish, operate tcpConn is relatively safe.

    // new resource
    _socketPtr = socket;
    _eventorPtr = make_shared<Eventor>(_socketPtr);
    _loopPtr = loop;

    memset(_sockReadBuffer, '\0', _SockBufferSize);
    memset(_sockWriteBuffer, '\0', _SockBufferSize);

    _readCallback = nullptr;
    _writeCallback = nullptr;
    _closeCallback = nullptr;

    _readBuffer.abort(_readBuffer.size());
    _writeBuffer.abort(_writeBuffer.size());

    _loopPtr->execInLoop(std::bind(&TcpConnection::reusedInLoop2, shared_from_this(), cb));

    LOG_NORMAL << "A tcpconnection reused.";
}


void TcpConnection::reusedInLoop2(InitedCallback cb)
{
    if(!turnTo(CONNECTED)) {
        LOG_WARN << "Turn to CONNECTED failed.";
        return;
    }

    _loopPtr->registerEventorImidiate(_eventorPtr);

    if(cb)
        cb(shared_from_this());
}


bool TcpConnection::turnTo(TcpConnState state)
{
    if(state == UNDEFINED)
        return false;
    else if(state == CLOSED) {
        if(_state == CONNECTED) {
            _state = CLOSED;
            return true;
        }
        else
            return false;
    }
    else if(state == WAITINIT) {
        if(_state == UNDEFINED || _state == CLOSED) {
            _state = WAITINIT;
            return true;
        }
        else 
            return false;
    }
    else if(state == CONNECTED) {
        if(_state == WAITINIT) {
            _state = CONNECTED;
            return true;
        }
        else
            return false;
    }
    return false;
}

bool TcpConnection::checkState(TcpConnState state)
{
    return _state == state;
}