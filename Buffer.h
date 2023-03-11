#ifndef IMITATER_BUFFER_H
#define IMITATER_BUFFER_H

#include <mutex>

namespace imitater
{
class Buffer
{
public:
    Buffer();
    ~Buffer();

    void read(void* data, int len);
    void write(void* data, int len);
    void pickRead(void* data, int len);
    void attach(int len);   // just postpone endpos
    void abort(int len);

    int size() const; // content size
private:
    char* _buffer;
    int _size;  // initiate 1024 bytes
    int _head;
    int _tail;
    std::mutex _mutex;

    int maxSize() const;   // max size
};
}

#endif