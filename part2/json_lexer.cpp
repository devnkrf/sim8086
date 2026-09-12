#include <cctype>
#include <cstdio>

#include "String.h"

const String trueStr = makeString("true", 4);
const String falseStr = makeString("false", 5);
const String nullStr = makeString("null", 4);

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

  printf("Tokens:\n");
  for (int i = 0; i < fileBuffer->size; i++) {
    // Braces
    if (fileBuffer->value[i] == '{') {
      printf("START_CURLY_BRACE ");
    } else if (fileBuffer->value[i] == '}') {
      printf("END_CURLY_BRACE ");
    } else if (fileBuffer->value[i] == '[') {
      printf("START_SQUARE_BRACE ");
    } else if (fileBuffer->value[i] == ']') {
      printf("END_SQUARE_BRACE ");
    }
    // Seperators
    else if (fileBuffer->value[i] == ',') {
      printf("COMMA ");
    } else if (fileBuffer->value[i] == ':') {
      printf("COLON ");
    }
    // Number, String, Boolean, Null
    else if (isdigit(fileBuffer->value[i]) || fileBuffer->value[i] == '-') {
      int start = i;
      while (i + 1 < fileBuffer->size && (isdigit(fileBuffer->value[i + 1]) ||
                                          fileBuffer->value[i + 1] == '.')) {
        i++;
      }
      printf("NUMBER(%.*s) ", i - start + 1, fileBuffer->value + start);
    } else if (fileBuffer->value[i] == '"') {
      int start = i;
      do {
        i++;
      } while (i < fileBuffer->size && fileBuffer->value[i] != '"');
      printf("STRING(%.*s) ", i - start + 1, fileBuffer->value + start);
    } else if (fileBuffer->value[i] == 't') {
      if (!lexer_tokenize_bool(fileBuffer, trueStr, i)) {
        printf("\nLexer Error: Unkown t at _,_ \n");
        return;
      }
      i += 3;
      printf("VAL_TRUE ");
    } else if (fileBuffer->value[i] == 'f') {
      // false
      if (!lexer_tokenize_bool(fileBuffer, falseStr, i)) {
        printf("\nLexer Error: Unkown f at _,_ \n");
        return;
      }
      i += 4;
      printf("VAL_FALSE ");
    } else if (fileBuffer->value[i] == 'n') {
      // null
      if (!lexer_tokenize_bool(fileBuffer, nullStr, i)) {
        printf("\nLexer Error: Unkown n at _,_ \n");
        return;
      }
      i += 3;
      printf("VAL_NULL ");
    }

    // Skip Whitespace
    else if (fileBuffer->value[i] == ' ') {
    } else if (fileBuffer->value[i] == '\n') {
    } else if (fileBuffer->value[i] == '\r') {
    } else if (fileBuffer->value[i] == '\t') {
    }
  }
}
