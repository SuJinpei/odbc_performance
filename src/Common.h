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
#define localtime_s(tm, t) localtime_r(t, tm)
#endif

#endif // !COMMON_H

