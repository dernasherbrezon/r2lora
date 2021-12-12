#ifndef Util_h
#define Util_h

#include <stdint.h>
#include <stddef.h>

int convertStringToHex(const char *str, uint8_t **output, size_t *output_len);

#endif