#include <cstdio>
#include <sys/stat.h>

#include "String.cpp"
#include "haversine_reference_calculator.cpp"
#include "json_parser.cpp"
#include "time_counter.cpp"
#include "time_counter.h"

struct HaversinePair {
  double x0, y0, x1, y1;
};

String readJsonFile(const char *file_name) {
  TimeFunction;
  String buffer;
  FILE *fptr;
  fptr = fopen(file_name, "rb");
  if (fptr) {
    struct stat Stat;
    stat(file_name, &Stat);
    buffer.value = (char *)malloc(Stat.st_size);
    if (buffer.value) {
      buffer.size = Stat.st_size;
      size_t bytes_read = fread(buffer.value, 1, buffer.size, fptr);
      if (bytes_read != buffer.size) {
        fprintf(stderr, "Unable to read from file:%s", file_name);
        free(buffer.value);
        buffer.size = 0;
      }
    } else {
      fprintf(stderr, "Unable to allocate buffer for size:%ld", Stat.st_size);
    }
    fclose(fptr);
  }
  return buffer;
}

std::vector<HaversinePair> parseHaversinePair(String fileBuffer) {
  TimeFunction;
  JsonNode json = parseJson(fileBuffer);
  // printJsonNode(json);
  std::vector<HaversinePair> result;
  JsonNode pairs = getFromJsonObject(json, CONSTANT_STRING("pairs"));
  JsonNode pairsArr = pairs.children[0];
  for (const auto &pair : pairsArr.children) {
    double x0 =
        getFromJsonObject(pair, CONSTANT_STRING("x0")).children[0].token.num;
    double y0 =
        getFromJsonObject(pair, CONSTANT_STRING("y0")).children[0].token.num;
    double x1 =
        getFromJsonObject(pair, CONSTANT_STRING("x1")).children[0].token.num;
    double y1 =
        getFromJsonObject(pair, CONSTANT_STRING("y1")).children[0].token.num;
    result.push_back({x0, y0, x1, y1});
  }
  return result;
}

double calculateHaversine(const std::vector<HaversinePair> &pairs) {
  TimeFunction;
  double result = 0;
  int n = pairs.size();
  if (n == 0) {
    return result;
  }
  for (const auto &pair : pairs) {
    result += ReferenceHaversine(pair.x0, pair.y0, pair.x1, pair.y1, 6372.8);
  }
  return result / n;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [haversine_input.json]\n", argv[0]);
    fprintf(stderr, "Usage: %s [haversine_input.json] [answers.f64]\n",
            argv[0]);
    return 1;
  }
  startProfiler();
  String fileBuffer = readJsonFile(argv[1]);
  auto jsonPairs = parseHaversinePair(fileBuffer);
  double haversine = calculateHaversine(jsonPairs);
  fprintf(stdout, "Calculated Haversine: %f\n", haversine);
  endProfiler();
  return 0;
}
