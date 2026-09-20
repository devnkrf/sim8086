#include <cstdio>
#include <sys/time.h>
#include <x86intrin.h>

#include "time_counter.h"

#define MAX_PROFILE_TIMER 4096

u64 GetOSTimerFreq() { return 1e6; }

u64 ReadOSTimer() {
  struct timeval Value;
  gettimeofday(&Value, 0);

  u64 Result = GetOSTimerFreq() * (u64)Value.tv_sec + (u64)Value.tv_usec;
  return Result;
}

inline u64 ReadCPUTimer() { return __rdtsc(); }

u64 EstimateCPUTimerFreq() {
  u64 MillisecondsToWait = 100;
  u64 OSFreq = GetOSTimerFreq();
  u64 CPUStart = ReadCPUTimer();
  u64 OSStart = ReadOSTimer();
  u64 OSEnd = 0;
  u64 OSElapsed = 0;
  u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
  while (OSElapsed < OSWaitTime) {
    OSEnd = ReadOSTimer();
    OSElapsed = OSEnd - OSStart;
  }
  u64 CPUEnd = ReadCPUTimer();
  u64 CPUElapsed = CPUEnd - CPUStart;
  u64 CPUFreq = OSFreq * CPUElapsed / OSElapsed;
  return CPUFreq;
}

struct TimerVals {
  u64 elapsedTime;
  u64 hitCount;
  const char *name;
};

struct Profiler {
  TimerVals timers[MAX_PROFILE_TIMER];
  u64 startTime;
  u64 endTime;
  u64 cpuFreq;
};

static Profiler GlobalProfiler;

TimeTaker::TimeTaker(const char *tname, u32 index) {
  if (index < MAX_PROFILE_TIMER) {
    startTime = ReadCPUTimer();
    name = tname;
    idx = index;
  } else {
    fprintf(stderr, "TimeTaker::index higher than MAX_PROFILE_TIMER");
  }
}
TimeTaker::~TimeTaker() {
  TimerVals *timer = GlobalProfiler.timers + idx;
  timer->name = name;
  timer->elapsedTime += ReadCPUTimer() - startTime;
  timer->hitCount++;
}

void printTimerMetric(TimerVals *timer, u64 totalTime) {
  fprintf(stdout, "%s: %lfs (%.2f%%)\n", timer->name,
          (double)timer->elapsedTime / GlobalProfiler.cpuFreq,
          ((double)timer->elapsedTime / totalTime) * 100);
}

void startProfiler() { GlobalProfiler.startTime = ReadCPUTimer(); }

void endProfiler() {
  GlobalProfiler.endTime = ReadCPUTimer();
  GlobalProfiler.cpuFreq = EstimateCPUTimerFreq();
  u64 totalTime = GlobalProfiler.endTime - GlobalProfiler.startTime;
  for (int i = 0; i < MAX_PROFILE_TIMER; i++) {
    TimerVals *timer = GlobalProfiler.timers + i;
    if (timer->elapsedTime) {
      printTimerMetric(timer, totalTime);
    } else {
      break;
    }
  }
}
