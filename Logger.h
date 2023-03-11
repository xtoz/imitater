#ifndef IMITATER_LOGGER_H
#define IMITATER_LOGGER_H

#include "Buffer.h"
#include <initializer_list>


#define PREFIX_LEN 200

namespace imitater
{
class Logger
{
public:
    static Logger* getInstace();
    static void release();

    void log(std::initializer_list<const char*> il);

private:
    Logger();
    ~Logger();
    static Logger* loggerInstance;
    Buffer _buffer;
};

class LoggerAgent
{
public:
    enum LogLevel {FATAL,ERRORme,WARN,NORMAL};  // seems cannot use 'ERROR', compiler will call false, do not know why.

    explicit LoggerAgent(const char* fileName, char codeLine, const char* function, LogLevel level);

    const char* levelToStr(LogLevel level);

    LoggerAgent& operator<<(const char* str);
    
private:
    char _prefix[PREFIX_LEN];
};
}

#define LOG_NORMAL imitater::LoggerAgent(__FILE__, __LINE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::NORMAL)
#define LOG_WARN imitater::LoggerAgent(__FILE__, __LINE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::WARN)
#define LOG_ERROR imitater::LoggerAgent(__FILE__, __LINE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::ERRORme)
#define LOG_FATAL imitater::LoggerAgent(__FILE__, __LINE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::FATAL)

#endif