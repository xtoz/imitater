#include "Socket.h"
#include "Logger.h"
#include <io.h>
#include <string>
#include <iostream>

using namespace imitater;
using namespace std;

const int Socket::DEFAULT_MAX_CONN = 20;
const unsigned short Socket::DEFAULT_PORT = 8888;


Socket::Socket()
{
    _sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(!isValid())
    {
        string log = "create invalid socket, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
    unsigned long ul = 1;
    // ioctlsocket(SOCKET s, long cmd, u_long FAR* argp);
    // s：一个标识套接口的描述字。cmd：对套接口s的操作命令。argp：指向cmd命令所带参数的指针。
    // 成功后返回0。否则的话，返回SOCKET_ERROR错误，应用程序可通过WSAGetLastError()获取相应错误代码。
    // FIONBIO：允许或禁止套接口的非阻塞模式。argp指向一个无符号长整型。如允许非阻塞模式则非零，如禁止非阻塞模式则为零。
    if(0 != ioctlsocket(_sockfd, FIONBIO, (unsigned long*)&ul))
    {
        string log = "set noblock socket fail, fd:";
        LOG_ERROR << log.c_str();
    }

    string log = "a socket constructed, fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
}

Socket::~Socket()
{
    close();
    string log = "a socket deconstructed, fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
}

Socket::Socket(int sockfd, sockaddr_in addr) :
_sockfd(sockfd),
_addr(_addr)
{
    if (!isValid())
    {
        string log = "a socket init with a invalid fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
    unsigned long ul = 1;
    if(0 != ioctlsocket(_sockfd, FIONBIO, (unsigned long*)&ul))
    {
        string log = "set noblock socket fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }

    string log = "a socket constructed with fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
}

void Socket::listen(unsigned short port, int maxConn)
{
    bind(nullptr, DEFAULT_PORT);
    if(SOCKET_ERROR == ::listen(_sockfd, DEFAULT_MAX_CONN))
    {
        string log = "start listen fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
}

Socket::SocketPtr Socket::accept()
{
    sockaddr_in addr;
    int size = sizeof(addr);
    int conn = ::accept(_sockfd, (sockaddr*)&addr, &size);
    if (INVALID_SOCKET == conn)
    {
        string log = "accept a invalid socket, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
    Socket::SocketPtr connSock = make_shared<Socket>(conn, addr);
    return connSock;
}

void Socket::bind(const char* host, unsigned short port)
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(SOCKET_ERROR == ::bind(_sockfd, (sockaddr*)&_addr, sizeof(_addr)))
    {
        string log = "socket bind fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
}

int Socket::read(void* data, int len)
{
    int size = ::recv(_sockfd, (char*)data, len, 0);
	if(-1 == size)
    {
        string log = "recieve data size < 0, fd:" + to_string(_sockfd);
		LOG_WARN << log.c_str();
	}
    else if (0 == size)
    {
        string log = "recieve data size = 0, fd:" + to_string(_sockfd);
        LOG_WARN << log.c_str();
    }
    else
    {
        string log = "recieve data size > 0, fd:" + to_string(_sockfd);
		LOG_NORMAL << log.c_str();
	}
    return size;
}

int Socket::write(void* data, int len)
{
    int size = ::send(_sockfd, (char*)data, len, 0);
    if(-1 == size)
    {
        string log = "send data size -1 failed, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
    else if(len == size)
    {
        string log = "send data size > 0 success, fd:" + to_string(_sockfd);
        LOG_NORMAL << log.c_str();
    }
    else
    {
        string log = "send part of data, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
    return size;
}

void Socket::close()
{
    string log = "a socket close, fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
    
    ::closesocket(_sockfd);
}

int Socket::sockFD() const
{
    return _sockfd;
}

bool Socket::isValid() const
{
    return _sockfd != INVALID_SOCKET;
}