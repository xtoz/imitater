#include "Buffer.h"
#include <string.h>
#include <iostream>
using namespace imitater;
using namespace std;

Buffer::Buffer()
{
    _size = 1024;
    _endPos = 0;
    _buffer = new char[_size];
    ::memset(_buffer, 0, _size);
}

Buffer::~Buffer()
{
    delete[] _buffer;
}

void Buffer::read(void* data, int len)
{
    lock_guard<mutex> lock(_mutex);
    if(len > _endPos)
        len = _endPos;
    ::memcpy(data, _buffer, len);
}

void Buffer::write(void* data, int len)
{
    lock_guard<mutex> lock(_mutex);
    int newEndPos = _endPos + len;
    if(newEndPos > _size)
    {
        int newSize = ((newEndPos / 1024) + 1) * 1024;
        char* newBuffer = new char[newSize];
        if (nullptr == newBuffer)
        {
            // TODO:error
        }
        ::memset(newBuffer, 0, newSize);
        ::memcpy(newBuffer, _buffer, _endPos);
        delete[] _buffer;
        _buffer = newBuffer;
        _size = newSize;
    }
    ::memcpy(_buffer + _endPos, data, len);
    _endPos = newEndPos;
}

void Buffer::pickRead(void* data, int len)
{
    lock_guard<mutex> lock(_mutex);
    if(len > _endPos)
        len = _endPos;
    ::memcpy(data, _buffer, len);
    ::memcpy(_buffer, _buffer+len, _endPos-len);
    _endPos -= len;
}

void Buffer::attach(int len)
{
    lock_guard<mutex> lock(_mutex);

    int newEnd = _endPos + len;
    if(newEnd > _size)
        newEnd = _size;

    _endPos = newEnd;
}

void Buffer::abort(int len)
{
    lock_guard<mutex> lock(_mutex);

    if(len > _endPos)
        len = _endPos;
        
    ::memcpy(_buffer, _buffer+len, _endPos-len);
    _endPos -= len;
}

int Buffer::size() const
{
    return _size;
}

int Buffer::endPos() const
{
    return _endPos;
}