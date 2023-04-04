#ifndef IMITATER_LOGGER_H
#define IMITATER_LOGGER_H

#include "Buffer.h"
#include <initializer_list>
#include <iostream>


#define PREFIX_LEN 1024

namespace imitater
{
class Logger
{
public:
    static Logger* getInstace();
    static void release();

    void log(const char* log);

private:
    Logger();
    ~Logger();
    static Logger* loggerInstance;
    Buffer _buffer;
    std::mutex _logMtx;
};

class LoggerAgent
{
public:
    enum LogLevel {FATAL,ERRORme,WARN,NORMAL,DEBUG};  // seems cannot use 'ERROR', compiler will call false, do not know why.

    explicit LoggerAgent(const char* fileName, const char* function, LogLevel level);
    ~LoggerAgent();

    const char* levelToStr(LogLevel level);

    LoggerAgent& operator<<(const char* log);
    
private:
    char _prefix[PREFIX_LEN];
};
}

#define LOG_NORMAL imitater::LoggerAgent(__FILE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::NORMAL)
#define LOG_WARN imitater::LoggerAgent(__FILE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::WARN)
#define LOG_ERROR imitater::LoggerAgent(__FILE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::ERRORme)
#define LOG_FATAL imitater::LoggerAgent(__FILE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::FATAL)
#define LOG_DEBUG imitater::LoggerAgent(__FILE__, __FUNCTION__, imitater::LoggerAgent::LogLevel::DEBUG)

#endif