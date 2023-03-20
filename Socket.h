#ifndef IMITATER_SOCKET_H
#define IMITATER_SOCKET_H

#include "Uncopyable.h"

#include <winsock2.h> 
#include <memory>

namespace imitater
{
class Socket : uncopyable
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
};
}

#endif