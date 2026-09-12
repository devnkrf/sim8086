#include <cstdlib>
#include <cstring>

#include "String.h"

// remember to clear it
String makeString(const char *str, long size) {
  String myStr;
  myStr.value = (char *)malloc(size);
  memcpy(myStr.value, str, size);
  myStr.size = size;
  return myStr;
}

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
