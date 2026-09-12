#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include "String.cpp"
#include "json_lexer.cpp"

constexpr auto filePath = "test.json";

int readJsonFile(String *fileBuffer) {
  FILE *fptr = fopen(filePath, "rb");
  if (!fptr) {
    printf("Unable to open file: %s\n", filePath);
    return 1;
  }

  fseek(fptr, 0, SEEK_END);
  long size = ftell(fptr);
  rewind(fptr);
  if (size <= 0) {
    printf("Empty File: %s\n", filePath);
    return 1;
  }

  fileBuffer->value = (char *)malloc(size);
  if (!fileBuffer->value) {
    printf("Unable to allocate fileBuffer of size %ld\n", size);
    return 1;
  }

  size_t bytes_read = fread(fileBuffer->value, 1, size, fptr);
  if (bytes_read != (size_t)size) {
    printf("Bytes read [%ld] not equal to file size [%ld]\n", bytes_read, size);
    return 1;
  }
  fileBuffer->size = size;

  fclose(fptr);
  return 0;
}

int main() {
  String fileBuffer = {};
  int fr_res = readJsonFile(&fileBuffer);
  if (fr_res == 0) {
    lexer_tokenize(&fileBuffer);
  }
  if (fileBuffer.value) {
    free(fileBuffer.value);
  }
  return fr_res;
}
