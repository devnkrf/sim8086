#include <iostream>

// Range -90 to 90
long double GetRandomLatitude() {
  long double scale = (long double)rand() / (long double)RAND_MAX;
  long double num = -90.0L + scale * 180.0L;
  return num;
}

// Range -180 to 180
long double GetRandomLongitude() {
  long double scale = (long double)rand() / (long double)RAND_MAX;
  long double num = -180.0L + scale * 360.0L;
  return num;
}

int main (int argc, char** argv) {

  if (argc != 3) {
    printf("Usage: %s <seed> <number_of_points>\n", argv[0]);
    return 1;
  }

  unsigned int seed = (unsigned int)atoi(argv[1]);
  int n = atoi(argv[2]);

  srand(seed);

  printf("{\"pairs\":[\n");
  for (int i = 0; i < n; i++) {
    long double x0 = GetRandomLatitude();
    long double x1 = GetRandomLatitude();
    long double y0 = GetRandomLongitude();
    long double y1 = GetRandomLongitude();
    printf("{\"x0\":%.16Lf, \"y0\":%.16Lf, \"x1\":%.16Lf, \"y1\":%.16Lf}", x0, y0, x1, y1);
    if(i < n-1) printf(",");
    printf("\n");
  }
  printf("]}\n");

  return 0;
}
