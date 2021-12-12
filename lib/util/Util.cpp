#include "Util.h"

#include <esp32-hal-log.h>
#include <stdlib.h>
#include <string.h>

int convertStringToHex(const char *str, uint8_t **output, size_t *output_len) {
  size_t len = 0;
  size_t str_len = strlen(str);
  for (size_t i = 0; i < str_len; i++) {
    if (str[i] == ' ') {
      continue;
    }
    len++;
  }
  if (len % 2 != 0) {
    log_i("invalid str length");
    *output = NULL;
    *output_len = 0;
    return -1;
  }
  size_t bytes = len / 2;
  uint8_t *result = (uint8_t *)malloc(sizeof(uint8_t) * bytes);
  for (size_t i = 0, j = 0; i < str_len; i++) {
    uint8_t curByte = 0;
    char curChar = str[i];
    if (curChar == ' ') {
      continue;
    }
    curByte *= 16;
    if (curChar > '0' && curChar < '9') {
      curByte = curChar - '0';
    } else if (curChar >= 'A' && curChar <= 'F') {
      curByte = (curChar - 'A') + 10;
    } else if (curChar >= 'a' && curChar <= 'f') {
      curByte = (curChar - 'a') + 10;
    } else {
      log_i("invalid char: %s", curChar);
      *output = NULL;
      *output_len = 0;
      return -1;
    }
    j++;
    if (j % 2 == 0) {
      result[j / 2 - 1] = curByte;
      curByte = 0;
    }
  }
  *output = result;
  *output_len = bytes;
  return 0;
}
