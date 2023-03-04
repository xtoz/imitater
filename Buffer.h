#ifndef IMITATER_BUFFER_H
#define IMITATER_BUFFER_H

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
    void attach(int len);
    void abort(int len);

    int size() const;
    int endPos() const; // real size
private:
    char* _buffer;
    int _size;  // initiate 1024 bytes
    int _endPos;
};
}

#endif