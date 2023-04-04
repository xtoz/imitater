#include "Socket.h"
#include "Logger.h"
#include <io.h>
#include <string>
#include <iostream>

using namespace imitater;
using namespace std;

const int Socket::DEFAULT_MAX_CONN = 20;
const unsigned short Socket::DEFAULT_PORT = 8888;

static constexpr SocketState UNDEFINED = SocketState::SOCK_UNDEFINED;
static constexpr SocketState INVALID = SocketState::SOCK_INVALID;
static constexpr SocketState WAITBIND = SocketState::SOCK_WAITBIND;
static constexpr SocketState WAITLISTEN = SocketState::SOCK_WAITLISTEN;
static constexpr SocketState LISTENING = SocketState::SOCK_LISTENING;
static constexpr SocketState CONNECTED = SocketState::SOCK_CONNECTED;
static constexpr SocketState CLOSED = SocketState::SOCK_CLOSED;

Socket::Socket() : StateMachine<SocketState>(UNDEFINED)
{
    _sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(!isValid())
    {
        turnTo(INVALID);
        string log = "Create invalid socket, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
        return;
    }

    if(!turnTo(WAITBIND)) {
        LOG_WARN << "Turn to wait bind failed, fd: " << to_string(_sockfd).c_str();
        return;
    }
    
    unsigned long ul = 1;
    // ioctlsocket(SOCKET s, long cmd, u_long FAR* argp);
    // s：一个标识套接口的描述字。cmd：对套接口s的操作命令。argp：指向cmd命令所带参数的指针。
    // 成功后返回0。否则的话，返回SOCKET_ERROR错误，应用程序可通过WSAGetLastError()获取相应错误代码。
    // FIONBIO：允许或禁止套接口的非阻塞模式。argp指向一个无符号长整型。如允许非阻塞模式则非零，如禁止非阻塞模式则为零。
    if(0 != ioctlsocket(_sockfd, FIONBIO, (unsigned long*)&ul))
    {
        string log = "Set noblock socket fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }

    string log = "Socket constructed, fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
}

Socket::~Socket()
{
    close();
    string log = "a socket deconstructed, fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
}

Socket::Socket(int sockfd, sockaddr_in addr) : StateMachine<SocketState>(UNDEFINED),
_sockfd(sockfd),
_addr(_addr)
{
    if (!isValid())
    {
        turnTo(INVALID);
        string log = "Socket init with a invalid fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
        return;
    }
    
    if(!turnTo(CONNECTED)) {
        LOG_WARN << "Turns to connected failed, fd: " << to_string(_sockfd).c_str();
        return;
    }

    unsigned long ul = 1;
    if(0 != ioctlsocket(_sockfd, FIONBIO, (unsigned long*)&ul))
    {
        string log = "Set noblock socket fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
        return;
    }   

    string log = "Socket constructed with fd:" + to_string(_sockfd);
    LOG_NORMAL << log.c_str();
}

void Socket::listen(unsigned short port, int maxConn)
{
    bind(nullptr, DEFAULT_PORT);

    if(!turnTo(LISTENING)) {
        LOG_WARN << "Turn to LISTENING failed, fd: " << to_string(_sockfd).c_str();
        return;
    }

    if(SOCKET_ERROR == ::listen(_sockfd, DEFAULT_MAX_CONN))
    {
        string log = "Start listen fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
}

Socket::SocketPtr Socket::accept()
{
    if(!checkState(LISTENING))
        return nullptr;
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
    if(!turnTo(WAITLISTEN)) {
        LOG_WARN << "Turn to WAITLISTEN failed, fd: " << to_string(_sockfd).c_str();
        return;
    }
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(SOCKET_ERROR == ::bind(_sockfd, (sockaddr*)&_addr, sizeof(_addr)))
    {
        string log = "socket bind fail, fd:" + to_string(_sockfd);
        LOG_ERROR << log.c_str();
    }
}

bool Socket::turnTo(SocketState state)
{
    if(state == CLOSED) {
        if(_state == CONNECTED) {
            _state = CLOSED;
            return true;
        }
        else
            return false;
    }
    else if(state == INVALID) {
        if(_state == UNDEFINED) {
            _state = INVALID;
            return true;
        }
        else
            return false;
    }
    else if(state == WAITBIND) {
        if(_state == UNDEFINED) {
            _state = WAITBIND;
            return true;
        }
        else
            return false;
    }
    else if(state == WAITLISTEN) {
        if(_state == WAITBIND) {
            _state = WAITLISTEN;
            return true;
        }
        else
            return false;
    }
    else if(state == LISTENING) {
        if(_state == WAITLISTEN) {
            _state = LISTENING;
            return true;
        }
        else
            return false;
    }
    else if(state == CONNECTED) {
        if(_state == UNDEFINED ) {
            _state = CONNECTED;
            return true;
        }
        else
            return false;
    }
    else if(state == UNDEFINED) {
        return false;
    }
    return false;
}

bool Socket::checkState(SocketState state)
{
    return state == _state;
}

int Socket::read(void* data, int len)
{
    if(!checkState(CONNECTED)) {
        LOG_WARN << "state check failed, fd" << to_string(_sockfd).c_str();
        return -1;
    }
        
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
    if(!checkState(CONNECTED)) {
        LOG_WARN << "state check failed, fd" << to_string(_sockfd).c_str();
        return -1;
    }
    
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
    if(!turnTo(CLOSED))
        return;

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