
#include <stdbool.h>
#include <string.h>

#include "common/util.h"
#include "parser/token.h"

static const char* tokenTypeToString[] = {
    [TOKEN_FN] = "fn",
    [TOKEN_IF] = "if",
    [TOKEN_IN] = "in",
    [TOKEN_AS] = "as",
    [TOKEN_FOR] = "for",
    [TOKEN_GOTO] = "goto",
    [TOKEN_ELSE] = "else",
    [TOKEN_FROM] = "from",
    [TOKEN_CASE] = "case",
    [TOKEN_TRUE] = "true",
    [TOKEN_FALSE] = "false",
    [TOKEN_CONST] = "const",
    [TOKEN_USING] = "using",
    [TOKEN_BREAK] = "break",
    [TOKEN_SIMPLE] = "simple",
    [TOKEN_SWITCH] = "switch",
    [TOKEN_EXTERN] = "extern",
    [TOKEN_RETURN] = "return",
    [TOKEN_IMPORT] = "import",
    [TOKEN_EXPORT] = "export",
    [TOKEN_TYPEOF] = "typeof",
    [TOKEN_VOLATILE] = "volatile",
    [TOKEN_CONTINUE] = "continue",

    [TOKEN_AT] = "@",
    [TOKEN_DOLLAR] = "$",
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
    [TOKEN_LES_GTR] = "<>",
    [TOKEN_PLUS] = "+",
    [TOKEN_MINUS] = "-",
    [TOKEN_STAR] = "*",
    [TOKEN_SLASH] = "/",
    [TOKEN_PERCENT] = "%",
    [TOKEN_TILDE] = "~",
    [TOKEN_EXCL] = "!",
    [TOKEN_POINT] = ".",
    [TOKEN_DBL_POINT] = "..",
    [TOKEN_DBL_PLUS] = "++",
    [TOKEN_DBL_MINUS] = "--",
    [TOKEN_COLON] = ":",
    [TOKEN_DBL_COLON] = "::",
    [TOKEN_COMMA] = ",",
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
    [TOKEN_CHAR_CONST] = "character",
    [TOKEN_STRING_CONST] = "string",
    [TOKEN_IDENTIFIER] = "identifier",
    [TOKEN_CUSTOM_OPERATOR] = "custom operator",

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
            // if (strncmp(tokenTypeToString[TOKEN_FN], from, len) == 0) {
            //     *type = TOKEN_FN;
            //     return len;
            // }
            if (strncmp(tokenTypeToString[TOKEN_IF], from, len) == 0) {
                *type = TOKEN_IF;
                return len;
            }
            // if (strncmp(tokenTypeToString[TOKEN_IN], from, len) == 0) {
            //     *type = TOKEN_IN;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_AS], from, len) == 0) {
            //     *type = TOKEN_AS;
            //     return len;
            // }
            break;
        case 3:
            if (strncmp(tokenTypeToString[TOKEN_FOR], from, len) == 0) {
                *type = TOKEN_FOR;
                return len;
            }
            break;
        case 4:
            // if (strncmp(tokenTypeToString[TOKEN_GOTO], from, len) == 0) {
            //     *type = TOKEN_GOTO;
            //     return len;
            // }
            if (strncmp(tokenTypeToString[TOKEN_ELSE], from, len) == 0) {
                *type = TOKEN_ELSE;
                return len;
            }
            // if (strncmp(tokenTypeToString[TOKEN_FROM], from, len) == 0) {
            //     *type = TOKEN_FROM;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_CASE], from, len) == 0) {
            //     *type = TOKEN_CASE;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_TRUE], from, len) == 0) {
            //     *type = TOKEN_TRUE;
            //     return len;
            // }
            break;
        // case 5:
            // if (strncmp(tokenTypeToString[TOKEN_FALSE], from, len) == 0) {
            //     *type = TOKEN_FALSE;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_CONST], from, len) == 0) {
            //     *type = TOKEN_CONST;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_USING], from, len) == 0) {
            //     *type = TOKEN_USING;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_BREAK], from, len) == 0) {
            //     *type = TOKEN_BREAK;
            //     return len;
            // }
            // break;
        case 6:
            // if (strncmp(tokenTypeToString[TOKEN_SIMPLE], from, len) == 0) {
            //     *type = TOKEN_SIMPLE;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_SWITCH], from, len) == 0) {
            //     *type = TOKEN_SWITCH;
            //     return len;
            // }
            if (strncmp(tokenTypeToString[TOKEN_EXTERN], from, len) == 0) {
                *type = TOKEN_EXTERN;
                return len;
            }
            if (strncmp(tokenTypeToString[TOKEN_RETURN], from, len) == 0) {
                *type = TOKEN_RETURN;
                return len;
            }
            // if (strncmp(tokenTypeToString[TOKEN_IMPORT], from, len) == 0) {
            //     *type = TOKEN_IMPORT;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_EXPORT], from, len) == 0) {
            //     *type = TOKEN_EXPORT;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_TYPEOF], from, len) == 0) {
            //     *type = TOKEN_TYPEOF;
            //     return len;
            // }
            break;
        // case 8:
            // if (strncmp(tokenTypeToString[TOKEN_VOLATILE], from, len) == 0) {
            //     *type = TOKEN_VOLATILE;
            //     return len;
            // }
            // if (strncmp(tokenTypeToString[TOKEN_CONTINUE], from, len) == 0) {
            //     *type = TOKEN_CONTINUE;
            //     return len;
            // }
        }
        *type = TOKEN_IDENTIFIER;
        return len;
    } else if ((from[0] >= '0' && from[0] <= '9') || (from[0] == '.' && (from[1] >= '0' && from[1] <= '9'))) {
        int len = 0;
        bool is_float = false;
        if (from[1] == 'b') {
            len = 2;
            while (from[len] == '0' || from[len] == '1' || from[len] == '_') {
                len++;
            }
            *type = TOKEN_INT_CONST;
        } else if (from[1] == 'o') {
            len = 2;
            while ((from[len] >= '0' && from[len] <= '7') || from[len] == '_') {
                len++;
            }
            *type = TOKEN_INT_CONST;
        } else if (from[1] == 'h' || from[1] == 'x') {
            len = 2;
            while (isHexChar(from[len]) || from[len] == '_') {
                len++;
            }
            *type = TOKEN_INT_CONST;
        } else {
            while ((from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
                len++;
            }
            if (from[len] == '.') {
                len++;
                is_float = true;
                while ((from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
                    len++;
                }
            }
            if (from[len] == 'e' && (from[len + 1] == '-' || from[len + 1] == '+' || (from[len + 1] >= '0' && from[len + 1] <= '9') || from[len + 1] == '_')) {
                is_float = true;
                len++;
                if (from[len] == '+' || from[len] == '-') {
                    len++;
                }
                while ((from[len] >= '0' && from[len] <= '9') || from[len] == '_') {
                    len++;
                }
            }
        }
        if(!is_float) {
            if(from[len] == 'B') {
                len++;
            } else if(from[len] == 'H') {
                while(from[len] == 'H') {
                    len++;
                }
            } else if(from[len] == 'L') {
                while(from[len] == 'L') {
                    len++;
                }
            }
        }
        *type = is_float ? TOKEN_FLOAT_CONST : TOKEN_INT_CONST;
        return len;
    } else if (from[0] == '\'') {
        int len = 1;
        while (from[len] != '\'' && from[len] != 0) {
            if (from[len] == '\\' && from[len + 1] == '\'') {
                len++;
            }
            len++;
        }
        if (from[len] == 0) {
            *type = TOKEN_INVALID;
            return len;
        }
        len++;
        *type = TOKEN_CHAR_CONST;
        return len;
    } else if (from[0] == '\"') {
        int len = 1;
        while (from[len] != '\"' && from[len] != 0) {
            if (from[len] == '\\' && from[len + 1] == '\"') {
                len++;
            }
            len++;
        }
        if (from[len] == 0) {
            *type = TOKEN_INVALID;
            return len;
        }
        len++;
        *type = TOKEN_STRING_CONST;
        return len;
    } else if (from[0] == '`') {
        int len = 1;
        bool valid = true;
        while (from[len] != '`' && from[len] != 0) {
            if (from[len] == '\\' && from[len + 1] == '`') {
                len++;
            }
            if (!((from[0] >= 'a' && from[0] <= 'z') || (from[0] >= 'A' && from[0] <= 'Z') || from[0] == '_')) {
                valid = false;
            }
            len++;
        }
        if (from[len] == 0 || !valid) {
            *type = TOKEN_INVALID;
            return len;
        }
        len++;
        *type = TOKEN_STRING_CONST;
        return len;
    } else {
        switch (from[0]) {
        case '@':
            *type = TOKEN_AT;
            return 1;
        case '$':
            *type = TOKEN_DOLLAR;
            return 1;
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
            case '>':
                *type = TOKEN_LES_GTR;
                return 2;
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
            switch (from[1]) {
            case '.':
                *type = TOKEN_DBL_POINT;
                return 2;
            default:
                *type = TOKEN_POINT;
                return 1;
            }
        case ':':
            switch (from[1]) {
            case '=':
                *type = TOKEN_COLON_EQU;
                return 2;
            case ':':
                *type = TOKEN_DBL_COLON;
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