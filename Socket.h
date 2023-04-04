#ifndef IMITATER_SOCKET_H
#define IMITATER_SOCKET_H

#include "Uncopyable.h"
#include "StateMachine.h"

#include <winsock2.h> 
#include <memory>

namespace imitater
{
enum SocketState {SOCK_UNDEFINED, SOCK_INVALID, SOCK_WAITBIND, SOCK_WAITLISTEN, SOCK_LISTENING, SOCK_CONNECTED, SOCK_CLOSED};
class Socket : uncopyable, public StateMachine<SocketState>
{
public:
    Socket();
    Socket(int sockfd, sockaddr_in addr);
    ~Socket();

    typedef std::shared_ptr<Socket> SocketPtr;

    void listen(unsigned short port=DEFAULT_PORT, int maxConn=DEFAULT_MAX_CONN);
    SocketPtr accept();
    int read(void* data, int len);
    int write(void* data, int len);
    void close();

    int sockFD() const;
    bool isValid() const;

private:
    
    static const int DEFAULT_MAX_CONN;    // 20
    static const unsigned short DEFAULT_PORT;  // 8888
    
    SOCKET _sockfd;
    sockaddr_in _addr;

    void bind(const char* host, unsigned short port);
    bool turnTo(SocketState state) override;
    bool checkState(SocketState state);
};
}

#endif