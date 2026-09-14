#ifndef TIME_COUNTER_H
#define TIME_COUNTER_H

#include "String.h"
#include "myTypes.h"

#define TakeTimeMetric(str) TimeTaker T(str);
#define TimeFunction TakeTimeMetric(__func__)

class TimeTaker {
private:
  u64 startTime;
  String name;

public:
  TimeTaker(const char *name);
  ~TimeTaker();
};

#endif // TIME_COUNTER_H
