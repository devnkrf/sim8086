#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "String.h"

String makeString(const char *str, long size) {
  String myStr;
  myStr.value = (char *)malloc(size);
  memcpy(myStr.value, str, size);
  myStr.size = size;
  return myStr;
}

String makeFromString(const char *str) { return makeString(str, strlen(str)); }

bool isStringEq(String A, String B) {
  if (A.size != B.size) {
    return false;
  }
  for (int i = 0; i < A.size; i++) {
    if (A.value[i] != B.value[i]) {
      return false;
    }
  }
  return true;
}

void printString(String A) { printf("%.*s", (int)A.size, A.value); }

void freeString(String &str) {
  if (str.value) {
    free(str.value);
  }
  str.value = NULL;
  str.size = 0;
}
