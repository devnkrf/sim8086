#ifndef MY_STRING_H
#define MY_STRING_H

struct String {
  char *value;
  long size;
};

String makeString(const char *str, long size);
bool isStringEq(String A, String B);

#endif // MY_STRING_H
