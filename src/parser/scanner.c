
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "parser/scanner.h"
#include "parser/token.h"

void initScanner(Scanner* scanner, const File* file) {
    scanner->file = file;
    scanner->current_offset = 0;
    scanner->token_cache_capacity = 0;
    scanner->token_cache_count = 0;
    scanner->token_cache_start = 0;
    scanner->token_cache_end = 0;
    scanner->token_cache = NULL;
}

void freeScanner(Scanner* scanner) {
    free(scanner->token_cache);
    scanner->token_cache = NULL;
}

static void pushTokenToCache(Scanner* scanner, Token token) {
    if (scanner->token_cache_count == scanner->token_cache_capacity) {
        scanner->token_cache_capacity = scanner->token_cache_capacity == 0 ? 4 : 2 * scanner->token_cache_capacity;
        scanner->token_cache = (Token*)realloc(scanner->token_cache, sizeof(Token) * scanner->token_cache_capacity);
        if (scanner->token_cache_end <= scanner->token_cache_start) {
            memcpy(scanner->token_cache + scanner->token_cache_count, scanner->token_cache, sizeof(Token) * scanner->token_cache_end);
            scanner->token_cache_end = scanner->token_cache_start + scanner->token_cache_count;
        }
    }
    scanner->token_cache[scanner->token_cache_end] = token;
    scanner->token_cache_end = (scanner->token_cache_end + 1) % scanner->token_cache_capacity;
    scanner->token_cache_count++;
}

static void popTokensFromCache(Scanner* scanner) {
    scanner->token_cache_start = (scanner->token_cache_start + 1) % scanner->token_cache_capacity;
    scanner->token_cache_count--;
}

static Token getFromCache(Scanner* scanner, int i) { return scanner->token_cache[(scanner->token_cache_start + i) % scanner->token_cache_capacity]; }

static bool skipWhitespace(Scanner* scanner) {
    bool ret = false;
    while (isspace(scanner->file->data[scanner->current_offset])) {
        scanner->current_offset++;
        ret = true;
    }
    return ret;
}

static bool skipComments(Scanner* scanner) {
    char* data = scanner->file->data + scanner->current_offset;
    int offset = 0;
    if (data[0] == '/' && data[1] == '/') {
        data += 2;
        offset += 2;
        while (data[0] != 0 && data[0] != '\n') {
            data++;
            offset++;
        }
        if (data[0] != 0) {
            data++;
            offset++;
        }
        scanner->current_offset += offset;
        return true;
    }
    if (data[0] == '/' && data[1] == '*') {
        data += 2;
        offset += 2;
        int layered = 0;
        while (data[0] != 0 && (data[0] != '*' || data[1] != '/' || layered)) {
            if (data[0] == '/' && data[1] == '*') {
                layered++;
            } else if (data[0] == '*' && data[1] == '/') {
                layered--;
            }
            data++;
            offset++;
        }
        if (data[0] != 0) {
            data += 2;
            offset += 2;
        }
        scanner->current_offset += offset;
        return true;
    }
    return false;
}

static void scanNextToken(Scanner* scanner) {
    Token next_token;
    while (skipWhitespace(scanner) || skipComments(scanner)) {
    }
    next_token.start = offsetToPosition(scanner->file, scanner->current_offset);
    int len = determineToken(scanner->file->data + scanner->current_offset, &(next_token.type));
    scanner->current_offset += len;
    next_token.end = offsetToPosition(scanner->file, scanner->current_offset);
    pushTokenToCache(scanner, next_token);
}

bool testForToken(Scanner* scanner, int n, TokenType type) {
    for (int i = scanner->token_cache_count; i <= n; i++) {
        scanNextToken(scanner);
    }
    Token token = getFromCache(scanner, n);
    return type == token.type;
}

bool acceptToken(Scanner* scanner, TokenType type) {
    if (scanner->token_cache_count == 0) {
        scanNextToken(scanner);
    }
    Token token = getFromCache(scanner, 0);
    bool ret = type == token.type;
    if (ret) {
        popTokensFromCache(scanner);
    }
    return ret;
}

bool consumeToken(Scanner* scanner, TokenType type, Token* token) {
    if (scanner->token_cache_count == 0) {
        scanNextToken(scanner);
    }
    Token tmp_token;
    if (token == NULL) {
        token = &tmp_token;
    }
    *token = getFromCache(scanner, 0);
    bool ret = type == token->type;
    if (ret) {
        (*token).type = type;
        popTokensFromCache(scanner);
    }
    return ret;
}

void getToken(Scanner* scanner, Token* token) {
    if (scanner->token_cache_count == 0) {
        scanNextToken(scanner);
    }
    *token = getFromCache(scanner, 0);
    popTokensFromCache(scanner);
}

Position getCurrentScannerPosition(Scanner* scanner) {
    if(scanner->token_cache_count == 0) {
        scanNextToken(scanner);
    }
    return scanner->token_cache[scanner->token_cache_start].start;
}
