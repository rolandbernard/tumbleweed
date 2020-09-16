
#include <stdbool.h>
#include <string.h>

#include "common/util.h"
#include "parser/token.h"

static const char* tokenTypeToString[] = {
    [TOKEN_IF] = "if",
    [TOKEN_FOR] = "for",
    [TOKEN_ELSE] = "else",
    [TOKEN_EXTERN] = "extern",
    [TOKEN_RETURN] = "return",

    [TOKEN_DBL_AND] = "&&",
    [TOKEN_DBL_PIPE] = "||",
    [TOKEN_DBL_EQU] = "==",
    [TOKEN_EXCL_EQU] = "!=",
    [TOKEN_GTR_EQU] = ">=",
    [TOKEN_LES_EQU] = "<=",
    [TOKEN_GTR] = ">",
    [TOKEN_LES] = "<",
    [TOKEN_PIPE] = "|",
    [TOKEN_AND] = "&",
    [TOKEN_CARET] = "^",
    [TOKEN_DBL_GTR] = ">>",
    [TOKEN_DBL_LES] = "<<",
    [TOKEN_PLUS] = "+",
    [TOKEN_MINUS] = "-",
    [TOKEN_STAR] = "*",
    [TOKEN_SLASH] = "/",
    [TOKEN_PERCENT] = "%",
    [TOKEN_TILDE] = "~",
    [TOKEN_EXCL] = "!",
    [TOKEN_POINT] = ".",
    [TOKEN_DBL_PLUS] = "++",
    [TOKEN_DBL_MINUS] = "--",
    [TOKEN_COLON] = ":",
    [TOKEN_SEMICOLON] = ";",
    [TOKEN_ROUND_OPEN] = "(",
    [TOKEN_ROUND_CLOSE] = ")",
    [TOKEN_SQUARE_OPEN] = "[",
    [TOKEN_SQUARE_CLOSE] = "]",
    [TOKEN_CURLY_OPEN] = "{",
    [TOKEN_CURLY_CLOSE] = "}",
    [TOKEN_COLON_EQU] = ":=",
    [TOKEN_EQU] = "=",
    [TOKEN_PLUS_EQU] = "+=",
    [TOKEN_MINUS_EQU] = "-=",
    [TOKEN_STAR_EQU] = "*=",
    [TOKEN_SLASH_EQU] = "/=",
    [TOKEN_PERCENT_EQU] = "%=",
    [TOKEN_PIPE_EQU] = "|=",
    [TOKEN_AND_EQU] = "&=",
    [TOKEN_CARET_EQU] = "^=",
    [TOKEN_DBL_GTR_EQU] = ">>=",
    [TOKEN_DBL_LES_EQU] = "<<=",

    [TOKEN_INT_CONST] = "integer",
    [TOKEN_FLOAT_CONST] = "float",
    [TOKEN_IDENTIFIER] = "identifier",

    [TOKEN_INVALID] = "invalid token",
    [TOKEN_ANY] = "any token",
    [TOKEN_EOF] = "end of file",
    [TOKEN_NONE] = "none",
};

bool testToken(const char* from, TokenType type) {
    TokenType test;
    determineToken(from, &test);
    return test == type ? true : false;
}

int determineToken(const char* from, TokenType* type) {
    if (from[0] == 0) {
        *type = TOKEN_EOF;
        return 0;
    } else if ((from[0] >= 'a' && from[0] <= 'z') || (from[0] >= 'A' && from[0] <= 'Z') || from[0] == '_') {
        int len = 1;
        while ((from[len] >= 'a' && from[len] <= 'z') || (from[len] >= 'A' && from[len] <= 'Z') || (from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
            len++;
        }
        switch (len) {
        case 2:
            if (strncmp(tokenTypeToString[TOKEN_IF], from, len) == 0) {
                *type = TOKEN_IF;
                return len;
            }
            break;
        case 3:
            if (strncmp(tokenTypeToString[TOKEN_FOR], from, len) == 0) {
                *type = TOKEN_FOR;
                return len;
            }
            break;
        case 4:
            if (strncmp(tokenTypeToString[TOKEN_ELSE], from, len) == 0) {
                *type = TOKEN_ELSE;
                return len;
            }
            break;
        case 6:
            if (strncmp(tokenTypeToString[TOKEN_EXTERN], from, len) == 0) {
                *type = TOKEN_EXTERN;
                return len;
            }
            if (strncmp(tokenTypeToString[TOKEN_RETURN], from, len) == 0) {
                *type = TOKEN_RETURN;
                return len;
            }
            break;
        }
        *type = TOKEN_IDENTIFIER;
        return len;
    } else if ((from[0] >= '0' && from[0] <= '9') || (from[0] == '.' && (from[1] >= '0' && from[1] <= '9'))) {
        if (from[1] == 'b') {
            int len = 2;
            while (from[len] == '0' || from[len] == '1' || from[len] == '_') {
                len++;
            }
            *type = TOKEN_INT_CONST;
            return len;
        }
        if (from[1] == 'o') {
            int len = 2;
            while ((from[len] >= '0' && from[len] <= '7') || from[len] == '_') {
                len++;
            }
            *type = TOKEN_INT_CONST;
            return len;
        }
        if (from[1] == 'h') {
            int len = 2;
            while (isHexChar(from[len]) || from[len] == '_') {
                len++;
            }
            *type = TOKEN_INT_CONST;
            return len;
        }
        int len = 0;
        bool is_float = false;
        while ((from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
            len++;
        }
        if (from[len] == '.') {
            is_float = true;
            while ((from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
                len++;
            }
        }
        if (from[len] == 'e') {
            is_float = true;
            if (from[len] == '+' || from[len] == '-') {
                len++;
            }
            while ((from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
                len++;
            }
        }
        *type = is_float ? TOKEN_FLOAT_CONST : TOKEN_INT_CONST;
        return len;
    } else {
        switch (from[0]) {
        case '&':
            switch (from[1]) {
            case '&':
                *type = TOKEN_DBL_AND;
                return 2;
            case '=':
                *type = TOKEN_AND_EQU;
                return 2;
            default:
                *type = TOKEN_AND;
                return 1;
            }
        case '|':
            switch (from[1]) {
            case '|':
                *type = TOKEN_DBL_PIPE;
                return 2;
            case '=':
                *type = TOKEN_PIPE_EQU;
                return 2;
            default:
                *type = TOKEN_PIPE;
                return 1;
            }
        case '=':
            switch (from[1]) {
            case '=':
                *type = TOKEN_DBL_EQU;
                return 2;
            default:
                *type = TOKEN_EQU;
                return 1;
            }
        case '>':
            switch (from[1]) {
            case '>':
                switch (from[2]) {
                case '=':
                    *type = TOKEN_DBL_GTR_EQU;
                    return 3;
                default:
                    *type = TOKEN_DBL_GTR;
                    return 2;
                }
            case '=':
                *type = TOKEN_GTR_EQU;
                return 2;
            default:
                *type = TOKEN_GTR;
                return 1;
            }
        case '<':
            switch (from[1]) {
            case '<':
                switch (from[2]) {
                case '=':
                    *type = TOKEN_DBL_LES_EQU;
                    return 3;
                default:
                    *type = TOKEN_DBL_LES;
                    return 2;
                }
            case '=':
                *type = TOKEN_LES_EQU;
                return 2;
            default:
                *type = TOKEN_LES;
                return 1;
            }
        case '^':
            switch (from[1]) {
            case '=':
                *type = TOKEN_CARET_EQU;
                return 2;
            default:
                *type = TOKEN_CARET;
                return 1;
            }
        case '+':
            switch (from[1]) {
            case '+':
                *type = TOKEN_DBL_PLUS;
                return 2;
            case '=':
                *type = TOKEN_PLUS_EQU;
                return 2;
            default:
                *type = TOKEN_PLUS;
                return 1;
            }
        case '-':
            switch (from[1]) {
            case '-':
                *type = TOKEN_DBL_MINUS;
                return 2;
            case '=':
                *type = TOKEN_MINUS_EQU;
                return 2;
            default:
                *type = TOKEN_MINUS;
                return 1;
            }
        case '*':
            switch (from[1]) {
            case '=':
                *type = TOKEN_STAR_EQU;
                return 2;
            default:
                *type = TOKEN_STAR;
                return 1;
            }
        case '/':
            switch (from[1]) {
            case '=':
                *type = TOKEN_SLASH_EQU;
                return 2;
            default:
                *type = TOKEN_SLASH;
                return 1;
            }
        case '%':
            switch (from[1]) {
            case '=':
                *type = TOKEN_PERCENT_EQU;
                return 2;
            default:
                *type = TOKEN_PERCENT;
                return 1;
            }
        case '~':
            *type = TOKEN_TILDE;
            return 1;
        case '!':
            switch (from[1]) {
            case '=':
                *type = TOKEN_EXCL_EQU;
                return 2;
            default:
                *type = TOKEN_EXCL;
                return 1;
            }
        case '.':
            *type = TOKEN_POINT;
            return 1;
        case ':':
            switch (from[1]) {
            case '=':
                *type = TOKEN_COLON_EQU;
                return 2;
            default:
                *type = TOKEN_COLON;
                return 1;
            }
        case ',':
            *type = TOKEN_COMMA;
            return 1;
        case ';':
            *type = TOKEN_SEMICOLON;
            return 1;
        case '(':
            *type = TOKEN_ROUND_OPEN;
            return 1;
        case ')':
            *type = TOKEN_ROUND_CLOSE;
            return 1;
        case '[':
            *type = TOKEN_SQUARE_OPEN;
            return 1;
        case ']':
            *type = TOKEN_SQUARE_CLOSE;
            return 1;
        case '{':
            *type = TOKEN_CURLY_OPEN;
            return 1;
        case '}':
            *type = TOKEN_CURLY_CLOSE;
            return 1;
        default:
            *type = TOKEN_INVALID;
            return 0;
        }
    }
}

const char* getTokenName(TokenType type) { return tokenTypeToString[type]; }
