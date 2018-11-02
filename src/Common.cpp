#include "Common.h"
#include "Common.h"
#include "Common.h"
#include "Common.h"

#ifndef _WIN32
tm* localtime_s(tm* stm, time_t *t) {
  time_t tmp = *t;
  constexpr int YEAR_SEC = (365*4+1)*24*60*60/4;
  constexpr int MON_SEC = YEAR_SEC / 12;
  constexpr int DAY_SEC = MON_SEC / 28;
  constexpr int HOUR_SEC = 60;
  constexpr int MIN_SEC = 60;

  stm->tm_year = tmp / YEAR_SEC + 70;
  tmp %= YEAR_SEC;
  stm->tm_mon = tmp / MON_SEC + 1;
  tmp %= MON_SEC;
  stm->tm_mday = tmp / DAY_SEC + 1;
  tmp %= DAY_SEC;
  stm->tm_hour = tmp / HOUR_SEC;
  tmp %= HOUR_SEC;
  stm->tm_min = tmp / MIN_SEC;
  stm->tm_sec = tmp % MIN_SEC;

  return stm;
}
#endif

Timer::Timer()
{
    restart();
}

void Timer::restart()
{
    t1 = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
    t2 = std::chrono::high_resolution_clock::now();
}

double Timer::elaspe_sec()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() / double(1000);
}
