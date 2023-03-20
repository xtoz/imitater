#ifndef IMITATER_EVENTOR_H
#define IMITATER_EVENTOR_H

#include "Socket.h"
#include "Uncopyable.h"
#include <functional>

namespace imitater
{
class Eventor : uncopyable
{
public:
    explicit Eventor(Socket::SocketPtr socket);
    ~Eventor();

    typedef std::shared_ptr<Eventor> EventorPtr;

    typedef std::function<void(void)> ReadCallback;
    typedef std::function<void(void)> WriteCallback;

    static const int EventNone;
    static const int EventRead;
    static const int EventWrite;

    void setReadCallback(ReadCallback cb);
    void setWriteCallback(WriteCallback cb);

    void setEvents(int events);
    int events() const;
    void setRevents(int revents);
    int revents() const;
    void update();
    int sockFD() const;
    int eid() const;    // global identification

    void handleEvents();

private:
    const Socket::SocketPtr _socketPtr;
    int _events;
    int _revents;

    ReadCallback _readCallback;
    WriteCallback _writeCallback;
};
}

#endif