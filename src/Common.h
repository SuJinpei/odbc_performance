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

#ifndef _WIN32
tm* localtime_s(tm* stm, time_t *t);
#endif

#endif // !COMMON_H

