#include <cstdio>
#include <sys/stat.h>

#include "String.cpp"
#include "haversine_reference_calculator.cpp"
#include "json_parser.cpp"
#include "time_counter.cpp"

struct HaversinePair {
  double x0, y0, x1, y1;
};

String readJsonFile(const char *file_name) {
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

std::vector<HaversinePair> parseHaversinePair(JsonNode json) {
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
  u64 time_start = ReadCPUTimer();

  String fileBuffer = readJsonFile(argv[1]);
  u64 time_file_read = ReadCPUTimer();

  JsonNode json = parseJson(fileBuffer);
  u64 time_json_parse = ReadCPUTimer();

  // printJsonNode(json);
  // u64 time_printJson = ReadCPUTimer();

  auto jsonPairs = parseHaversinePair(json);
  u64 time_pair_parse = ReadCPUTimer();

  double haversine = calculateHaversine(jsonPairs);
  u64 time_haversine = ReadCPUTimer();

  u64 cpuFreq = EstimateCPUTimerFreq();
  fprintf(stdout, "Calculated Haversine: %f\n", haversine);

  u64 totalTime = time_haversine - time_start;
  auto timeInSeconds = [cpuFreq](u64 time) -> double {
    return (double)time / cpuFreq;
  };
  auto timeInPercentage = [totalTime](u64 time) -> double {
    return ((double)time / totalTime) * 100;
  };
  auto printTimeMetrics = [&](u64 time) -> void {
    fprintf(stdout, "%lf (%.2f%)\n", timeInSeconds(time),
            timeInPercentage(time));
  };
  fprintf(stdout, "Total time taken: ");
  printTimeMetrics(totalTime);

  u64 readTime = time_file_read - time_start;
  fprintf(stdout, "Read File: ");
  printTimeMetrics(readTime);

  u64 jsonParseTime = time_json_parse - time_file_read;
  fprintf(stdout, "Json Parse: ");
  printTimeMetrics(jsonParseTime);

  u64 haversineParseTime = time_pair_parse - time_json_parse;
  fprintf(stdout, "Haversine Parse: ");
  printTimeMetrics(haversineParseTime);

  u64 totalParseTime = time_pair_parse - time_file_read;
  fprintf(stdout, "Total Parse: ");
  printTimeMetrics(totalParseTime);

  u64 haversineTime = time_haversine - time_pair_parse;
  fprintf(stdout, "Haversine Calculation: ");
  printTimeMetrics(haversineTime);

  return 0;
}
