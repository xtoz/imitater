#include "Buffer.h"
#include "Logger.h"
#include <string.h>
#include <iostream>
using namespace imitater;
using namespace std;

Buffer::Buffer()
{
    _size = 1024;
    _head = 0;
    _tail = 0;
    _buffer = new char[_size];
    memset(_buffer, 0, _size);
}

Buffer::~Buffer()
{
    delete[] _buffer;
}

void Buffer::read(void* data, int len)
{
    if (len < 0)
        return;

    lock_guard<mutex> lock(_mutex);

    if(len > _tail - _head)
        len = _tail - _head;
    memcpy(data, _buffer + _head, len);
}

void Buffer::write(void* data, int len)
{
    if (len < 0)
        return;

    lock_guard<mutex> lock(_mutex);
    
    if(_tail + len > _size)
    {
        int newSize = (((_tail + len) / 1024) + 1) * 1024;
        char* newBuffer = new char[newSize];
        if (nullptr == newBuffer)
        {
            LOG_ERROR << "buffer expand fail.";
            return;
        }
        memset(newBuffer, 0, newSize);
        memcpy(newBuffer, _buffer + _head, _tail - _head);
        delete[] _buffer;
        _buffer = newBuffer;
        _size = newSize;
    }
    memcpy(_buffer + _head, data, len);
    _tail += len;
}

void Buffer::pickRead(void* data, int len)
{
    if (len < 0)
        return;

    lock_guard<mutex> lock(_mutex);

    if(len > _tail - _head)
        len = _tail - _head;
    memcpy(data, _buffer + _head, len);
    _head += len;
    if(_head == _tail)
        _head = _tail = 0;
}

void Buffer::attach(int len)
{
    if (len < 0)
        return;

    lock_guard<mutex> lock(_mutex);

    if(_tail + len > _size)
        _tail = _size;
    else
        _tail += len;
}

void Buffer::abort(int len)
{
    if (len < 0)
        return;
        
    lock_guard<mutex> lock(_mutex);

    if(len > _tail - _head)
        _head = _tail = 0;
    else
        _head += len;
}

int Buffer::maxSize() const
{
    return _size;
}

int Buffer::size() const
{
    return _tail - _head;
}