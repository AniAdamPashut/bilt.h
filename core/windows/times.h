#ifndef BASE_H
# include "core/base.h"
#endif

#ifdef PLATFORM_WIN

#include <windows.h>
#include <winnt.h>

i64 TimeNow() {
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  // Convert Windows FILETIME (100-nanosecond intervals since January 1, 1601)
  // to UNIX timestamp in milliseconds
  i64 currentTime = (li.QuadPart - 116444736000000000LL) / 10000;
  return currentTime;
}

void WaitTime(i64 ms) {
  Sleep(ms);
}

#endif