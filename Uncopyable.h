#ifndef UNCOPYABLE_H
#define UNCOPYABLE_H

class uncopyable
{
protected:
    uncopyable()=default;   // avoid derectly instanslize uncopyable
    ~uncopyable()=default;
public:
    uncopyable(const uncopyable&) = delete;
    uncopyable operator=(const uncopyable&) = delete;
    uncopyable(const uncopyable&&) = delete;
    uncopyable operator=(const uncopyable&&) = delete;
};

#endif