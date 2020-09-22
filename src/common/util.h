#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int Position;

#define NOPOS -1

#define PARSE_ERROR (void*)1

#define max(X, Y) ((X) > (Y) ? (X) : (Y))
#define min(X, Y) ((X) < (Y) ? (X) : (Y))

bool isHexChar(char c);

int hexCharToInt(char c);

char intToHexChar(int n);

int parseUTF8(char* c, int* length);

int printUTF8(int c, char* out);

void uuidv4(char* ret);

#define COMPILER_NAME "tumbleweed 0.0.1"

#endif
