#include <cctype>
#include <cstdio>
#include <vector>

#include "String.h"

const String trueStr = makeString("true", 4);
const String falseStr = makeString("false", 5);
const String nullStr = makeString("null", 4);

enum class TOKEN_TYPE {
  START_CURLY_BRACE,
  END_CURLY_BRACE,
  START_SQUARE_BRACE,
  END_SQUARE_BRACE,
  COMMA,
  COLON,
  NUMBER,
  STRING,
  BOOLEAN,
  VAL_NULL,
  UNKNOWN
};

struct TOKEN {
  TOKEN_TYPE type;
  union {
    String str;
    bool boolean;
    double num;
  };
  int col, row;
};

void printToken(const TOKEN &token) {
  switch (token.type) {
  case TOKEN_TYPE::START_CURLY_BRACE:
    printf("START_CURLY_BRACE");
    break;
  case TOKEN_TYPE::END_CURLY_BRACE:
    printf("END_CURLY_BRACE");
    break;
  case TOKEN_TYPE::START_SQUARE_BRACE:
    printf("START_SQUARE_BRACE");
    break;
  case TOKEN_TYPE::END_SQUARE_BRACE:
    printf("END_SQUARE_BRACE");
    break;
  case TOKEN_TYPE::COMMA:
    printf("COMMA");
    break;
  case TOKEN_TYPE::COLON:
    printf("COLON");
    break;
  case TOKEN_TYPE::VAL_NULL:
    printf("VAL_NULL");
    break;
  case TOKEN_TYPE::BOOLEAN:
    printf("BOOLEAN(%s)", token.boolean ? "true" : "false");
    break;
  case TOKEN_TYPE::STRING:
    printf("STRING(%.*s)", (int)token.str.size, token.str.value);
    break;
  case TOKEN_TYPE::NUMBER:
    printf("NUMBER(%.*s)", (int)token.str.size, token.str.value);
    break;
  default:
    printf("UNKNOWN(%d,%d)", token.row, token.col);
  }
}

bool lexer_tokenize_bool(String *fileBuffer, String ref, int i) {
  if (i + ref.size > fileBuffer->size) {
    return false;
  }
  if (!isStringEq({fileBuffer->value + i, ref.size}, ref)) {
    return false;
  }
  return true;
}

void lexer_tokenize(String *fileBuffer) {
  printf("Original Json\n%.*s\n", (int)fileBuffer->size, fileBuffer->value);

  std::vector<TOKEN> tokens;
  TOKEN token = {.type = TOKEN_TYPE::UNKNOWN, .col = 0, .row = 0};
  for (int i = 0; i < fileBuffer->size; i++) {
    // Braces
    if (fileBuffer->value[i] == '{') {
      token.type = TOKEN_TYPE::START_CURLY_BRACE;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == '}') {
      token.type = TOKEN_TYPE::END_CURLY_BRACE;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == '[') {
      token.type = TOKEN_TYPE::START_SQUARE_BRACE;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == ']') {
      token.type = TOKEN_TYPE::END_SQUARE_BRACE;
      tokens.push_back(token);
    }
    // Seperators
    else if (fileBuffer->value[i] == ',') {
      token.type = TOKEN_TYPE::COMMA;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == ':') {
      token.type = TOKEN_TYPE::COLON;
      tokens.push_back(token);
    }
    // Number, String, Boolean, Null
    else if (isdigit(fileBuffer->value[i]) || fileBuffer->value[i] == '-') {
      int start = i;
      while (i + 1 < fileBuffer->size && (isdigit(fileBuffer->value[i + 1]) ||
                                          fileBuffer->value[i + 1] == '.')) {
        i++;
      }
      token.type = TOKEN_TYPE::NUMBER;
      token.str.value = fileBuffer->value + start;
      token.str.size = i - start + 1;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == '"') {
      int start = i;
      do {
        i++;
      } while (i < fileBuffer->size && fileBuffer->value[i] != '"');
      token.type = TOKEN_TYPE::STRING;
      token.str.value = fileBuffer->value + start;
      token.str.size = i - start + 1;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == 't') {
      if (!lexer_tokenize_bool(fileBuffer, trueStr, i)) {
        token.type = TOKEN_TYPE::UNKNOWN;
        tokens.push_back(token);
        break;
      }
      i += 3;
      token.row += 3;
      token.type = TOKEN_TYPE::BOOLEAN;
      token.boolean = true;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == 'f') {
      // false
      if (!lexer_tokenize_bool(fileBuffer, falseStr, i)) {
        token.type = TOKEN_TYPE::UNKNOWN;
        tokens.push_back(token);
        break;
      }
      i += 4;
      token.row += 4;
      token.type = TOKEN_TYPE::BOOLEAN;
      token.boolean = false;
      tokens.push_back(token);
    } else if (fileBuffer->value[i] == 'n') {
      // null
      if (!lexer_tokenize_bool(fileBuffer, nullStr, i)) {
        token.type = TOKEN_TYPE::UNKNOWN;
        tokens.push_back(token);
        break;
      }
      i += 3;
      token.row += 3;
      token.type = TOKEN_TYPE::VAL_NULL;
      tokens.push_back(token);
    }

    // New Line
    else if (fileBuffer->value[i] == '\n' || fileBuffer->value[i] == '\r') {
      token.col++;
      token.row = 0;
      continue;
    }
    // Skip Whitespace
    else if (fileBuffer->value[i] == ' ') {
    } else if (fileBuffer->value[i] == '\t') {
    }
    // Unknown
    else {
      token.type = TOKEN_TYPE::UNKNOWN;
      tokens.push_back(token);
      break;
    }
    token.row++;
  }
  printf("Tokens:\n");
  for (const auto &t : tokens) {
    printToken(t);
    printf(" ");
  }
  printf("\n");
}
