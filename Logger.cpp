#include "Logger.h"
#include <iostream>
#include <string.h>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace imitater;
using namespace std;

LoggerAgent::LoggerAgent(const char* fileName, const char* function, LogLevel level)
{
    ::memset(_prefix, '\0', PREFIX_LEN);

    // chrono::system_clock::time_point current = chrono::system_clock::now();
    // time_t now_time = chrono::system_clock::to_time_t(current);
    // tm* now_tm = std::localtime(&now_time);
    // strftime(_prefix, PREFIX_LEN, "%Y-%m-%d %H:%M:%S", now_tm);

    // time_t tm;
    // time(&tm);
    // strftime(_prefix, sizeof(_prefix), "%Y-%m-%d %H:%M:%S", localtime(&tm));

    // i do not know why above method get time will cause stack collapse.
    time_t now = time(0); char* dt = ctime(&now); dt[strlen(dt) - 1] = '\0';    // replace last char may be danger since strlen can return 0.

    // construct prefix: time + file + codeline + function
    sprintf_s(_prefix, sizeof(_prefix), "%s, Level: %s, File: %s, Function: %s. Content: ",
              dt, levelToStr(level), fileName, function);

    Logger::getInstace()->log(_prefix);
}

LoggerAgent::~LoggerAgent()
{
    Logger::getInstace()->log("\n");
}

const char *LoggerAgent::levelToStr(LoggerAgent::LogLevel level)
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
    case DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

LoggerAgent& LoggerAgent::operator<<(const char *log)
{
    // sprintf_s(_prefix + strlen(_prefix), sizeof(_prefix), "%s", log);
    // Logger::getInstace()->log(_prefix);
    Logger::getInstace()->log(log);
    return *this;
}

Logger* Logger::loggerInstance = new Logger(); // consider multi-thread;

Logger* Logger::getInstace()
{
    if(nullptr == loggerInstance)
        loggerInstance = new Logger();  // in theory, this cannot exec.
    return loggerInstance;
}

void Logger::release()
{
    if(nullptr != loggerInstance)
        delete loggerInstance;
}

Logger::Logger()
{

}

Logger::~Logger()
{

}

void Logger::log(const char* log)
{
    unique_lock<mutex> lock(_logMtx);
    cout << log;
}