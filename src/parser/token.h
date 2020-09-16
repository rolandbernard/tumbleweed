#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "driver/file.h"

typedef enum {
    TOKEN_NONE = 0,

    TOKEN_KEYWORD_START,

    TOKEN_IF,       // if
    TOKEN_FOR,      // for
    TOKEN_ELSE,     // else
    TOKEN_EXTERN,   // extern
    TOKEN_RETURN,   // return

    TOKEN_KEYWORD_END,
    TOKEN_OPERATOR_START,

    TOKEN_DBL_AND,      // &&
    TOKEN_DBL_PIPE,     // ||
    TOKEN_DBL_EQU,      // ==
    TOKEN_EXCL_EQU,     // !=
    TOKEN_GTR_EQU,      // >=
    TOKEN_LES_EQU,      // <=
    TOKEN_GTR,          // >
    TOKEN_LES,          // <
    TOKEN_PIPE,         // |
    TOKEN_AND,          // &
    TOKEN_CARET,        // ^
    TOKEN_DBL_GTR,      // >>
    TOKEN_DBL_LES,      // <<
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_PERCENT,      // %
    TOKEN_TILDE,        // ~
    TOKEN_EXCL,         // !
    TOKEN_POINT,        // .
    TOKEN_DBL_PLUS,     // ++
    TOKEN_DBL_MINUS,    // --
    TOKEN_COLON,        // :
    TOKEN_COMMA,        // ,
    TOKEN_SEMICOLON,    // ;
    TOKEN_ROUND_OPEN,   // (
    TOKEN_ROUND_CLOSE,  // )
    TOKEN_SQUARE_OPEN,  // [
    TOKEN_SQUARE_CLOSE, // ]
    TOKEN_CURLY_OPEN,   // {
    TOKEN_CURLY_CLOSE,  // }
    TOKEN_COLON_EQU,    // :=
    TOKEN_EQU,          // =
    TOKEN_PLUS_EQU,     // +=
    TOKEN_MINUS_EQU,    // -=
    TOKEN_STAR_EQU,     // *=
    TOKEN_SLASH_EQU,    // /=
    TOKEN_PERCENT_EQU,  // %=
    TOKEN_PIPE_EQU,     // |=
    TOKEN_AND_EQU,      // &=
    TOKEN_CARET_EQU,    // ^=
    TOKEN_DBL_GTR_EQU,  // >>=
    TOKEN_DBL_LES_EQU,  // <<=

    TOKEN_OPERATOR_END,
    TOKEN_SPECIAL_START,

    TOKEN_INT_CONST,    // 42
    TOKEN_FLOAT_CONST,  // 4.2
    TOKEN_IDENTIFIER,   // abc

    TOKEN_SPECIAL_END,

    TOKEN_INVALID,
    TOKEN_ANY,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    Position start;
    Position end;
} Token;

bool testToken(const char* from, TokenType type);

int determineToken(const char* from, TokenType* type);

const char* getTokenName(TokenType type);

#endif
