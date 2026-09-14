#ifndef MY_STRING_H
#define MY_STRING_H

struct String {
  char *value;
  long size;
};

#define CONSTANT_STRING(str) {(char *)(str), sizeof(str) - 1}

bool isStringEq(String A, String B);
void printString(String A);

// From any string (remember to free)
String makeString(const char *str, long size);
// From null terminated string (remember to free)
String makeFromString(const char *str);

void freeString(String &str);

#endif // MY_STRING_H
