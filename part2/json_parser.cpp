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

struct JsonNode {
  TOKEN token;
  std::vector<JsonNode> children;
};

void printEmptyVector() { printf("Empty token vector\n"); }

void printUnexpectedVector(TOKEN token) {
  printf("parse_value: Unexpected Token: ");
  printToken(token);
  printf(" at (%d,%d)\n", token.col, token.row);
}

void printIndent(int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
}

void printJsonNode(const JsonNode &root, int depth = 0) {
  switch (root.token.type) {
  case TOKEN_TYPE::BOOLEAN:
    printf("%s", root.token.boolean ? "true" : "false");
    break;
  case TOKEN_TYPE::STRING:
    printString(root.token.str);
    break;
  case TOKEN_TYPE::NUMBER:
    printf("%lf", root.token.num);
    break;
  case TOKEN_TYPE::VAL_NULL:
    printf("null");
    break;
  case TOKEN_TYPE::ARRAY: {
    printf("[");
    if (!root.children.empty()) {
      printf("\n");
      for (size_t i = 0; i < root.children.size(); i++) {
        printIndent(depth + 1);
        printJsonNode(root.children[i], depth + 1);
        printf("%s\n", i + 1 < root.children.size() ? "," : "");
      }
      printIndent(depth);
    }
    printf("]");
    break;
  }
  case TOKEN_TYPE::OBJECT: {
    printf("{");
    if (!root.children.empty()) {
      printf("\n");
      for (size_t i = 0; i < root.children.size(); i++) {
        const JsonNode &kv = root.children[i];
        printIndent(depth + 1);
        printJsonNode(kv, depth + 1);
        printf(": ");
        printJsonNode(kv.children[0], depth + 1);
        printf("%s\n", i + 1 < root.children.size() ? "," : "");
      }
      printIndent(depth);
    }
    printf("}");
    break;
  }
  default:
    printUnexpectedVector(root.token);
    break;
  }
}

// forward declaration
bool parse_value(const std::vector<TOKEN> &tokens, JsonNode &root, int &curr);
bool parse_array(const std::vector<TOKEN> &tokens, JsonNode &root, int &curr);
bool parse_object(const std::vector<TOKEN> &tokens, JsonNode &root, int &curr);

bool parse_value(const std::vector<TOKEN> &tokens, JsonNode &root, int &curr) {
  if (tokens.empty()) {
    printEmptyVector();
    return false;
  }
  TOKEN token = tokens[curr];
  switch (token.type) {
  case TOKEN_TYPE::START_CURLY_BRACE:
    return parse_object(tokens, root, curr);
  case TOKEN_TYPE::START_SQUARE_BRACE:
    return parse_array(tokens, root, curr);
  case TOKEN_TYPE::BOOLEAN:
  case TOKEN_TYPE::NUMBER:
  case TOKEN_TYPE::STRING:
  case TOKEN_TYPE::VAL_NULL:
    root.token = token;
    root.children = {};
    curr++;
    return true;
  default:
    printUnexpectedVector(token);
    return false;
  }
  return true;
}

bool parse_array(const std::vector<TOKEN> &tokens, JsonNode &root, int &curr) {
  // array - [ <value>? ,? <value>? ]
  if (tokens.empty()) {
    printEmptyVector();
    return false;
  }
  if (tokens[curr].type != TOKEN_TYPE::START_SQUARE_BRACE ||
      tokens.size() - curr < 2) {
    printUnexpectedVector(tokens[curr]);
    return false;
  }
  curr++;
  root.token.type = TOKEN_TYPE::ARRAY;
  if (tokens[curr].type == TOKEN_TYPE::END_SQUARE_BRACE) {
    curr++;
    return true;
  }
  do {
    JsonNode child;
    bool ret = parse_value(tokens, child, curr);
    if (!ret) {
      return ret;
    }
    root.children.push_back(child);
    if (tokens[curr].type == TOKEN_TYPE::COMMA) {
      curr++;
      continue;
    } else {
      break;
    }
  } while (true);
  if (tokens[curr].type == TOKEN_TYPE::END_SQUARE_BRACE) {
    curr++;
    return true;
  } else {
    printUnexpectedVector(tokens[curr]);
    return false;
  }
}

bool parse_object(const std::vector<TOKEN> &tokens, JsonNode &root, int &curr) {
  // object - { <string:value>? ,? <string:value>? }
  if (tokens.empty()) {
    printEmptyVector();
    return false;
  }
  if (tokens[curr].type != TOKEN_TYPE::START_CURLY_BRACE ||
      tokens.size() - curr < 2) {
    printUnexpectedVector(tokens[curr]);
    return false;
  }
  curr++;
  root.token.type = TOKEN_TYPE::OBJECT;
  if (tokens[curr].type == TOKEN_TYPE::END_CURLY_BRACE) {
    curr++;
    return true;
  }
  do {
    if (tokens[curr].type != TOKEN_TYPE::STRING) {
      printUnexpectedVector(tokens[curr]);
      return false;
    }
    JsonNode key;
    key.token = tokens[curr];
    curr++;
    if (tokens[curr].type != TOKEN_TYPE::COLON) {
      printUnexpectedVector(tokens[curr]);
      return false;
    }
    curr++;
    JsonNode child;
    bool ret = parse_value(tokens, child, curr);
    if (!ret) {
      return ret;
    }
    key.children.push_back(child);
    root.children.push_back(key);
    if (tokens[curr].type == TOKEN_TYPE::COMMA) {
      curr++;
      continue;
    } else {
      break;
    }
  } while (true);
  if (tokens[curr].type == TOKEN_TYPE::END_CURLY_BRACE) {
    curr++;
    return true;
  } else {
    printUnexpectedVector(tokens[curr]);
    return false;
  }
}

int main() {
  String fileBuffer = {};
  int fr_res = readJsonFile(&fileBuffer);
  if (fr_res == 0) {
    std::vector<TOKEN> tokens = lexer_tokenize(&fileBuffer);
    printf("Tokens:\n");
    for (const auto &t : tokens) {
      printToken(t);
      printf(" ");
    }
    printf("\n");

    printf("AST:\n");
    int curr = 0;
    JsonNode root;
    parse_value(tokens, root, curr);
    printJsonNode(root);
  }
  if (fileBuffer.value) {
    free(fileBuffer.value);
  }
  return fr_res;
}
