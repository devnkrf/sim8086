#include <cstdint>
#include <sys/time.h>
#include <x86intrin.h>

typedef uint64_t u64;
typedef double f64;

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
