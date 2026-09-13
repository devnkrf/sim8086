#include <cstdio>
#include <sys/stat.h>

#include "String.cpp"
#include "haversine_reference_calculator.cpp"
#include "json_parser.cpp"

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

double haversineDistance(JsonNode json) {
  double result = 0;
  JsonNode pairs = getFromJsonObject(json, CONSTANT_STRING("pairs"));
  JsonNode pairsArr = pairs.children[0];
  int n = pairsArr.children.size();
  for (const auto &pair : pairsArr.children) {
    double x0 =
        getFromJsonObject(pair, CONSTANT_STRING("x0")).children[0].token.num;
    double y0 =
        getFromJsonObject(pair, CONSTANT_STRING("y0")).children[0].token.num;
    double x1 =
        getFromJsonObject(pair, CONSTANT_STRING("x1")).children[0].token.num;
    double y1 =
        getFromJsonObject(pair, CONSTANT_STRING("y1")).children[0].token.num;
    result += ReferenceHaversine(x0, y0, x1, y1, 6372.8);
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
  String fileBuffer = readJsonFile(argv[1]);
  JsonNode json = parseJson(fileBuffer);
  // printJsonNode(json);
  fprintf(stdout, "Calculated Haversine: %f\n", haversineDistance(json));
  return 0;
}
