#ifndef TIME_COUNTER_H
#define TIME_COUNTER_H

#include "String.h"
#include "myTypes.h"

#define TakeTimeMetric(str, idx) TimeTaker T(str, idx);
#define TimeFunction TakeTimeMetric(__func__, __COUNTER__)

class TimeTaker {
private:
  u64 startTime;
  const char *name;
  u32 idx;

public:
  TimeTaker(const char *name, u32 idx);
  ~TimeTaker();
};

#endif // TIME_COUNTER_H
