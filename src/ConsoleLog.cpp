#include "ConsoleLog.h"


void ConsoleLog::setLevel(ConsoleLog::LogLevel lv)
{
    logLevel = lv;
}

ConsoleLog::Color::Color(int i)
{
    set_color(i);
}

ConsoleLog::Color::~Color()
{
#ifdef _WIN32
    auto h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 7);
#endif // _WIN32
}

void ConsoleLog::Color::set_color(int i)
{
#ifdef _WIN32
    auto h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, i);
#endif // _WIN32
}

ConsoleLog & console() {
    static ConsoleLog logger(ConsoleLog::LDEBUG);
    return logger;
}