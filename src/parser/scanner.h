#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "common/util.h"
#include "driver/file.h"
#include "parser/token.h"

typedef struct {
    const File* file;
    int current_offset;
    int token_cache_capacity;
    int token_cache_count;
    int token_cache_start;
    int token_cache_end;
    Token* token_cache;
} Scanner;

void initScanner(Scanner* scanner, const File* file);

void freeScanner(Scanner* scanner);

bool test(Scanner* scanner, int n, TokenType type);

bool accept(Scanner* scanner, TokenType type);

bool consume(Scanner* scanner, TokenType type, Token* token);

void getToken(Scanner* scanner, Token* token);

Position getCurrentScannerPosition(Scanner* scanner);

#endif
