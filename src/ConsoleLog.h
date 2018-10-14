#ifndef CONSOLE_LOG_H
#define CONSOLE_LOG_H

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include "Common.h"

#include <iostream>
#include <mutex>


class ConsoleLog {
public:
    enum LogLevel {
        LERROR,
        LWARNING,
        LINFO,
        LDEBUG
    };

    class Color {
    public:
        enum ColorEnum{GREEN = 2, RED = 4, PURPLE = 5, YELLOW = 6};

        Color(int i);
        ~Color();
        void set_color(int i);
    };

    ConsoleLog(const LogLevel lv = LERROR)
        :_buffer{ std::cout }, logLevel{ lv }{}

    template<ConsoleLog::LogLevel lv, typename... Args>
    void log(Args const&... args) {
        if (lv <= logLevel) {
            Color color(7);
            std::unique_lock<std::mutex> lck{ mutex_log };
            switch (lv) {
            case ConsoleLog::LERROR:
                color.set_color(Color::RED);
                _buffer << "[***ERROR***]\t"; break;
            case ConsoleLog::LWARNING:
                color.set_color(Color::YELLOW);
                _buffer << "[***WARNING***]\t"; break;
            default:
                break;
            }
            oss_output(_buffer, args...);
        }
    }

    template<ConsoleLog::LogLevel lv, typename... Args>
    void log_line(Args const&... args) {
        log<lv>(args...);
        oss_output(_buffer, "\n");
    }

    void setLevel(ConsoleLog::LogLevel lv);

private:
    std::ostream& _buffer;
    LogLevel logLevel;
    std::mutex mutex_log;
};

ConsoleLog& console();

#endif // CONSOLE_LOG_H
