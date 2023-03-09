#include "Logger.h"
#include <iostream>

using namespace imitater;
using namespace std;

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