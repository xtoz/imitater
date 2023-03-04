#include "Eventor.h"

using namespace imitater;

const int Eventor::EventNone = 0x000;
const int Eventor::EventRead = 0x001;
const int Eventor::EventWrite = 0x010;

Eventor::Eventor(Socket::SocketPtr socket) :
_socketPtr(socket)
{
    _events = EventNone;
    _revents = EventNone;
}

Eventor::~Eventor()
{

}

void Eventor::setReadCallback(ReadCallback cb)
{
    _readCallback = cb;
}

void Eventor::setWriteCallback(WriteCallback cb)
{
    _writeCallback = cb;
}

void Eventor::setEvents(int events)
{
    _events = events;
}

int Eventor::events() const
{
    return _events;
}

void Eventor::setRevents(int revents)
{
    _revents = revents;
}

int Eventor::revents() const
{
    return _revents;
}

void Eventor::update()
{

}

int Eventor::sockFD() const
{
    return _socketPtr->sockFD();
}

int Eventor::eid() const
{
    return _socketPtr->sockFD();
}

void Eventor::handleEvents()
{
    if((_revents & EventRead) && _readCallback)
    {
        _readCallback();
    }
    if((_revents & EventWrite) && _writeCallback)
    {
        _writeCallback();
    }
}