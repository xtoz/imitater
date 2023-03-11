#include "Logger.h"
#include <iostream>
#include <string.h>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace imitater;
using namespace std;

LoggerAgent::LoggerAgent(const char* fileName, char codeLine, const char* function, LogLevel level)
{
    ::memset(_prefix, PREFIX_LEN, '\0');

    // system time str
    chrono::system_clock::time_point now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm* now_tm = std::localtime(&now_time);
    strftime(_prefix, PREFIX_LEN, "%Y-%m-%d %H:%M:%S", now_tm);

    // construct prefix: time + file + codeline
    sprintf_s(_prefix + strlen(_prefix), sizeof(_prefix), ", Level: %s, File: %s, Line: %c, Function: %s. Content: ",
              levelToStr(level), fileName, codeLine, function);
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
    default:
        return "UNKNOWN";
    }
}

LoggerAgent& LoggerAgent::operator<<(const char *log)
{
    sprintf_s(_prefix + strlen(_prefix), sizeof(_prefix), "%s", log);
    Logger::getInstace()->log({_prefix});
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

void Logger::log(initializer_list<const char*> il)
{
    for (auto iter = il.begin(); iter != il.end(); ++iter)
		cout << *iter;
	cout << endl;
}