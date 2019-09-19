#ifndef COMMON_H
#define COMMON_H

#ifdef max
#undef max
#endif // max

//#include <gsl/gsl>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "error.h"

#include <sqlext.h>
#include <time.h>
#include <chrono>

#ifndef _WIN32
tm* localtime_s(tm* stm, time_t *t);
#endif

class Timer {
public:
    Timer();
    void restart();
    void stop();
    double elaspe_sec();

    static std::string get_format_timestamp(std::string fmt);

private:
#ifndef _WIN32
    std::chrono::time_point<std::chrono::system_clock> t1;
    std::chrono::time_point<std::chrono::system_clock> t2;
#else
    std::chrono::time_point<std::chrono::steady_clock> t1;
    std::chrono::time_point<std::chrono::steady_clock> t2;
#endif
};

#endif // !COMMON_H

