#ifndef LINUX_TIMES_H
#define LINUX_TIMES_H

#ifndef BASE_H
# include "core/base.h"
#endif

#ifdef PLATFORM_LINUX

#define CLOCK_REALTIME			0

i64 TimeNow() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  i64 currentTime = (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
  assert(currentTime != -1 && "currentTime should never be -1");
  return currentTime;
}

void WaitTime(i64 ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

#endif

#endif