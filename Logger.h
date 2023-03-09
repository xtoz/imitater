#ifndef IMITATER_LOGGER_H
#define IMITATER_LOGGER_H

#include "Buffer.h"
#include <string.h>
#include <initializer_list>
#include <windows.h>

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

    explicit LoggerAgent(const char* fileName, char codeLine, LogLevel level)
    {
        SYSTEMTIME time;
        ::GetLocalTime(&time);
        sprintf_s(_prefix, "%04d/%02d/%02d %02d:%02d:%02d, level: %s, file: %s, line: %c. ",
                  time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
                  levelToStr(level), fileName, codeLine, PREFIX_LEN);
    }

    const char* levelToStr(LogLevel level)
    {
        switch (level)
        {
        case FATAL:
            return "FATAL";
        case ERRORme:
            return "ERROR";
        case WARN:
            return "WARN";
        case NORMAL:
            return "NORMAL";
        default:
            return "UNKNOWN";
        }
    }

    LoggerAgent& operator<<(const char* str)
    {
        Logger::getInstace()->log({_prefix, str});
        return *this;
    }
    
private:
    char _prefix[PREFIX_LEN];
};
}

#define LOG_NORMAL imitater::LoggerAgent(__FILE__, __LINE__, imitater::LoggerAgent::LogLevel::NORMAL)
#define LOG_WARN imitater::LoggerAgent(__FILE__, __LINE__, imitater::LoggerAgent::LogLevel::WARN)
#define LOG_ERROR imitater::LoggerAgent(__FILE__, __LINE__, imitater::LoggerAgent::LogLevel::ERRORme)
#define LOG_FATAL imitater::LoggerAgent(__FILE__, __LINE__, imitater::LoggerAgent::LogLevel::FATAL)

#endif