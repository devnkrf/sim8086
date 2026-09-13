#include <cstdio>

#include "String.cpp"
#include "haversine_reference_calculator.cpp"
#include "json_parser.cpp"

constexpr auto file_name = "output.json";

// Range -90 to 90
double GetRandomLatitude() {
  double scale = (double)rand() / (double)RAND_MAX;
  double num = -90.0L + scale * 180.0L;
  return num;
}

// Range -180 to 180
double GetRandomLongitude() {
  double scale = (double)rand() / (double)RAND_MAX;
  double num = -180.0L + scale * 360.0L;
  return num;
}

double create_haversine_json(unsigned int seed, int n) {
  double result = 0;

  FILE *fptr;
  fptr = fopen(file_name, "w");

  srand(seed);

  fprintf(fptr, "{\"pairs\":[\n");
  for (int i = 0; i < n; i++) {
    double x0 = GetRandomLatitude();
    double x1 = GetRandomLatitude();
    double y0 = GetRandomLongitude();
    double y1 = GetRandomLongitude();
    result += ReferenceHaversine(x0, y0, x1, y1, 6372.8);
    fprintf(fptr, "{\"x0\":%.16f, \"y0\":%.16f, \"x1\":%.16f, \"y1\":%.16f}",
            x0, y0, x1, y1);
    if (i < n - 1)
      fprintf(fptr, ",");
    fprintf(fptr, "\n");
  }
  fprintf(fptr, "]}\n");
  fclose(fptr);

  return result / n;
}

int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s <seed> <number_of_points>\n", argv[0]);
    return 1;
  }

  unsigned int seed = (unsigned int)atoi(argv[1]);
  int n = atoi(argv[2]);

  printf("Random Seed: %d\n", seed);
  printf("Pair Count: %u\n", n);
  printf("Expected Sum: %f\n", create_haversine_json(seed, n));
  printf("Writing to: %s\n", file_name);

  return 0;
}
