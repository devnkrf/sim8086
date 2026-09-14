#include <cstdio>
#include <sys/time.h>
#include <vector>
#include <x86intrin.h>

#include "String.h"
#include "time_counter.h"

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

/*
int main(void) {
  auto CPUFreq = EstimateCPUTimerFreq();
  printf("CPU Seconds: %lu\n", CPUFreq);

  return 0;
}
*/
/*
 * constructor() -> take the name + take the start time
 * destructor()  -> take the end time + store somewhere??
 */

#define TakeTimeMetric(str) TimeTaker T(str);

struct TimerVals {
  u64 startTime, stopTime;
  String name;
};
std::vector<TimerVals> timeMetrics;
TimerVals totalTime{0, 0, CONSTANT_STRING("Total Time")};

TimeTaker::TimeTaker(const char *name) {
  startTime = ReadCPUTimer();
  this->name = makeFromString(name);
}
TimeTaker::~TimeTaker() {
  u64 stopTime = ReadCPUTimer();
  timeMetrics.push_back({startTime, stopTime, name});
}

void setupTimeTaker() {
  for (auto &t : timeMetrics) {
    freeString(t.name);
  }
  timeMetrics.clear();
  totalTime.startTime = ReadCPUTimer();
}

void printTimerMetric(TimerVals t, u64 cpuFreq, u64 total) {
  u64 timeTaken = t.stopTime - t.startTime;
  fprintf(stdout, "%.*s: %lfs (%.2f%%)\n", (int)t.name.size, t.name.value,
          (double)timeTaken / cpuFreq, ((double)timeTaken / total) * 100);
}

void endTimeTaker() {
  u64 cpuFreq = EstimateCPUTimerFreq();

  totalTime.stopTime = ReadCPUTimer();
  u64 total = totalTime.stopTime - totalTime.startTime;
  printTimerMetric(totalTime, cpuFreq, total);

  for (const auto &t : timeMetrics) {
    printTimerMetric(t, cpuFreq, total);
  }
}
