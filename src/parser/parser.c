
#include <stdio.h>
#include <stdlib.h>

#include "common/arraylist.h"
#include "driver/error.h"
#include "driver/file.h"
#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/scanner.h"

static int parseEscapeCode(char* data, int* length) {
    int ret;
    switch (data[0]) {
    case 'a':
        ret = '\a';
        *length = 1;
        break;
    case 'b':
        ret = '\b';
        *length = 1;
        break;
    case 't':
        ret = '\t';
        *length = 1;
        break;
    case 'n':
        ret = '\n';
        *length = 1;
        break;
    case 'v':
        ret = '\v';
        *length = 1;
        break;
    case 'f':
        ret = '\f';
        *length = 1;
        break;
    case 'r':
        ret = '\r';
        *length = 1;
        break;
    case 'e':
        ret = '\e';
        *length = 1;
        break;
    case 'x':
        if (isHexChar(data[1]) && isHexChar(data[2])) {
            ret = (hexCharToInt(data[1]) << 4) | hexCharToInt(data[2]);
            *length = 2;
        } else {
            ret = -1;
        }
        break;
    case 'u':
        if (isHexChar(data[1]) && isHexChar(data[2]) && isHexChar(data[3]) && isHexChar(data[4])) {
            ret = (hexCharToInt(data[1]) << 12) | (hexCharToInt(data[2]) << 8) | (hexCharToInt(data[3]) << 4) | hexCharToInt(data[4]);
            *length = 4;
        } else {
            ret = -1;
        }
        break;
    case 'U':
        if (isHexChar(data[1]) && isHexChar(data[2]) && isHexChar(data[3]) && isHexChar(data[4]) && isHexChar(data[5]) && isHexChar(data[6]) && isHexChar(data[7]) && isHexChar(data[8])) {
            ret = (hexCharToInt(data[1]) << 28) | (hexCharToInt(data[2]) << 24) | (hexCharToInt(data[3]) << 20) | (hexCharToInt(data[4]) << 16);
            ret = (hexCharToInt(data[5]) << 12) | (hexCharToInt(data[6]) << 8) | (hexCharToInt(data[7]) << 4) | hexCharToInt(data[8]);
            *length = 8;
        } else {
            ret = -1;
        }
        break;
    default:
        ret = data[0];
        *length = 1;
        break;
    }
    return ret;
}

static bool expectToken(Scanner* scanner, TokenType type, ErrorContext* error_context, Token* token) {
    Token tmp_token;
    if (token == NULL) {
        token = &tmp_token;
    }
    if (consume(scanner, type, token)) {
        return true;
    } else {
        addErrorf(error_context, token->start, ERROR, "Expected a '%s' but found a '%s'", getTokenName(type), getTokenName(token->type));
        return false;
    }
}

static Ast* parseExpression(Scanner* scanner, ErrorContext* error_context);

static AstCodeBlock* parseCodeBlock(Scanner* scanner, ErrorContext* error_context);

static Ast* parseStatment(Scanner* scanner, ErrorContext* error_context);

static AstRoot* parseRoot(Scanner* scanner, ErrorContext* error_context);

static Ast* parseType(Scanner* scanner, ErrorContext* error_context) { return parseExpression(scanner, error_context); }

static AstIfElse* parseIf(Scanner* scanner, ErrorContext* error_context) {
    Token first;
    if (consume(scanner, TOKEN_IF, &first)) {
        Ast* condition = NULL;
        Ast* if_block = NULL;
        Ast* else_block = NULL;
        condition = parseExpression(scanner, error_context);
        if (condition != NULL && condition != PARSE_ERROR) {
            if_block = (Ast*)parseStatment(scanner, error_context);
            if (if_block != NULL && if_block != PARSE_ERROR) {
                if (accept(scanner, TOKEN_ELSE)) {
                    else_block = (Ast*)parseStatment(scanner, error_context);
                    if (else_block == NULL || else_block == PARSE_ERROR) {
                        if (else_block == NULL) {
                            addError(error_context, "Expected a code block or another if condition", getCurrentScannerPosition(scanner), ERROR);
                        }
                        goto failed;
                    }
                }
                AstIfElse* ret = (AstIfElse*)malloc(sizeof(AstIfElse));
                ret->type = AST_IF_ELSE;
                ret->start = first.start;
                ret->end = else_block == NULL ? if_block->end : else_block->end;
                ret->condition = condition;
                ret->if_block = if_block;
                ret->else_block = else_block;
                return ret;
            } else {
                if (if_block == NULL) {
                    addError(error_context, "Expected a code block", getCurrentScannerPosition(scanner), ERROR);
                }
            }
        } else {
            if (condition == NULL) {
                addError(error_context, "Expected a condition", getCurrentScannerPosition(scanner), ERROR);
            }
        }
    failed:
        freeAst(condition);
        freeAst(if_block);
        return PARSE_ERROR;
    }
    return NULL;
}

static Ast* parseFor(Scanner* scanner, ErrorContext* error_context) {
    Token first;
    if (consume(scanner, TOKEN_FOR, &first)) {
        Ast* condition = NULL;
        Ast* body = NULL;
        condition = parseExpression(scanner, error_context);
        if (condition != PARSE_ERROR) {
            body = parseStatment(scanner, error_context);
            if (body != PARSE_ERROR) {
                AstForLoop* ret = (AstForLoop*)malloc(sizeof(AstForLoop));
                ret->type = AST_FOR_LOOP;
                ret->start = first.start;
                ret->end = body->end;
                ret->condition = condition;
                ret->code_block = body;
                return (Ast*)ret;
            }
        }
        freeAst(condition);
        freeAst((Ast*)body);
        return PARSE_ERROR;
    }
    return NULL;
}

// return
static Ast* parseDirective(Scanner* scanner, ErrorContext* error_context) {
    Token first;
    if (consume(scanner, TOKEN_RETURN, &first)) {
        Ast* value = parseExpression(scanner, error_context);
        if (value == PARSE_ERROR) {
            return PARSE_ERROR;
        }
        AstUnaryOperation* ret = (AstUnaryOperation*)malloc(sizeof(AstUnaryOperation));
        ret->type = AST_RETURN;
        ret->start = first.start;
        ret->end = value == NULL ? first.end : value->end;
        ret->operand = value;
        return (Ast*)ret;
    }
    return NULL;
}

static Ast* parseAssignmentOrVariableDefinitionOrExpression(Scanner* scanner, ErrorContext* error_context) {
    Token first;
    consume(scanner, TOKEN_NONE, &first);
    Ast* destination = parseExpression(scanner, error_context);
    if (destination == PARSE_ERROR) {
        goto failed;
    }
    if (destination == NULL) {
        return NULL;
    }
    Token action;
    if (accept(scanner, TOKEN_COLON)) {
        if (destination->type != AST_VARIABLE_ACCESS) {
            addError(error_context, "The expression is not a variable name", destination->start, ERROR);
            goto failed;
        }
        Ast* type = parseType(scanner, error_context);
        if (type == PARSE_ERROR || type == NULL) {
            if (type == NULL) {
                addError(error_context, "Expected a type", getCurrentScannerPosition(scanner), ERROR);
            }
            goto failed_definition;
        }            
        if (accept(scanner, TOKEN_EQU)) {
            Ast* value = parseExpression(scanner, error_context);
            if (value == PARSE_ERROR || value == NULL) {
                if (value == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed_definition_values;
            }
            AstVariableDefinition* ret = (AstVariableDefinition*)malloc(sizeof(AstVariableDefinition));
            ret->type = AST_VARIABLE_DEFINITION;
            ret->start = first.start;
            ret->end = ((Ast*)value)->end;
            ret->is_extern = false;
            ret->name = ((AstVariableAccess*)destination)->name;
            free(destination);
            ret->variable_type = (Ast*)type;
            ret->initial_value = (Ast*)value;
            return (Ast*)ret;
        failed_definition_values:
            freeAst((Ast*)value);
            goto failed_definition;
        } else {
            AstVariableDefinition* ret = (AstVariableDefinition*)malloc(sizeof(AstVariableDefinition));
            ret->type = AST_VARIABLE_DEFINITION;
            ret->start = first.start;
            ret->end = ((Ast*)type)->end;
            ret->is_extern = false;
            ret->name = ((AstVariableAccess*)destination)->name;
            free(destination);
            ret->variable_type = (Ast*)type;
            ret->initial_value = NULL;
            return (Ast*)ret;
        }
    failed_definition:
        freeAst((Ast*)type);
        goto failed;
    } else if (consume(scanner, TOKEN_EQU, &action) || consume(scanner, TOKEN_PIPE_EQU, &action) || consume(scanner, TOKEN_AND_EQU, &action) || consume(scanner, TOKEN_CARET_EQU, &action) || consume(scanner, TOKEN_DBL_GTR_EQU, &action) || consume(scanner, TOKEN_DBL_LES_EQU, &action) || consume(scanner, TOKEN_PLUS_EQU, &action) || consume(scanner, TOKEN_MINUS_EQU, &action) || consume(scanner, TOKEN_STAR_EQU, &action) || consume(scanner, TOKEN_SLASH_EQU, &action) || consume(scanner, TOKEN_PERCENT_EQU, &action) || consume(scanner, TOKEN_COLON_EQU, &action)) {
        if (action.type == TOKEN_COLON_EQU) {
            if (destination->type != AST_VARIABLE_ACCESS) {
                addError(error_context, "The expression is not a variable name", destination->start, ERROR);
                goto failed;
            }
        }
        Ast* value = parseExpression(scanner, error_context);
        if (value == PARSE_ERROR || value == NULL) {
            if (value == NULL) {
                addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
            }
            goto failed_assign;
        }
        if (action.type == TOKEN_COLON_EQU) {
            AstVariableDefinition* ret = (AstVariableDefinition*)malloc(sizeof(AstVariableDefinition));
            ret->type = AST_VARIABLE_DEFINITION;
            ret->start = first.start;
            ret->end = ((Ast*)value)->end;
            ret->is_extern = false;
            ret->name = ((AstVariableAccess*)destination)->name;
            free(destination);
            ret->variable_type = NULL;
            ret->initial_value = (Ast*)value;
            return (Ast*)ret;
        } else {
            AstAssignment* ret = (AstAssignment*)malloc(sizeof(AstAssignment));
            switch (action.type) {
            case TOKEN_EQU:
                ret->type = AST_ASSIGNMENT;
                break;
            case TOKEN_PIPE_EQU:
                ret->type = AST_OR_ASSIGNMENT;
                break;
            case TOKEN_AND_EQU:
                ret->type = AST_AND_ASSIGNMENT;
                break;
            case TOKEN_CARET_EQU:
                ret->type = AST_XOR_ASSIGNMENT;
                break;
            case TOKEN_DBL_GTR_EQU:
                ret->type = AST_SHIFT_RIGHT_ASSIGNMENT;
                break;
            case TOKEN_DBL_LES_EQU:
                ret->type = AST_SHIFT_LEFT_ASSIGNMENT;
                break;
            case TOKEN_PLUS_EQU:
                ret->type = AST_ADD_ASSIGNMENT;
                break;
            case TOKEN_MINUS_EQU:
                ret->type = AST_SUBTRACT_ASSIGNMENT;
                break;
            case TOKEN_STAR_EQU:
                ret->type = AST_MULTIPLY_ASSIGNMENT;
                break;
            case TOKEN_SLASH_EQU:
                ret->type = AST_DIVIDE_ASSIGNMENT;
                break;
            case TOKEN_PERCENT_EQU:
                ret->type = AST_REMAINDER_ASSIGNMENT;
                break;
            default:
                break;
            }
            ret->start = first.start;
            ret->end = ((Ast*)value)->end;
            ret->destination = destination;
            ret->value = value;
            return (Ast*)ret;
        }
    failed_assign:
        freeAst((Ast*)value);
        goto failed;
    } else {
        return (Ast*)destination;
    } 
failed:
    freeAst((Ast*)destination);
    return PARSE_ERROR;
}

static Ast* parseStatment(Scanner* scanner, ErrorContext* error_context) { 
    Ast* ret = NULL;
    (ret = (Ast*)parseIf(scanner, error_context)) != NULL ||
    (ret = (Ast*)parseFor(scanner, error_context)) != NULL ||
    (ret = (Ast*)parseDirective(scanner, error_context)) != NULL ||
    (ret = (Ast*)parseCodeBlock(scanner, error_context)) != NULL ||
    (ret = (Ast*)parseAssignmentOrVariableDefinitionOrExpression(scanner, error_context)) != NULL;
    return ret;
}

static AstCodeBlock* parseCodeBlock(Scanner* scanner, ErrorContext* error_context) {
    Token first;
    if (consume(scanner, TOKEN_CURLY_OPEN, &first)) {
        ArrayList statments;
        initArrayList(&statments);
        Ast* statment = NULL;
        while ((statment = parseStatment(scanner, error_context)) != NULL || (statment = (Ast*)parseCodeBlock(scanner, error_context)) != NULL) {
            if (statment == PARSE_ERROR) {
                goto failed;
            }
            pushToArrayList(&statments, (void*)statment);
            while (accept(scanner, TOKEN_SEMICOLON))
                ;
        }
        Token last;
        if (expectToken(scanner, TOKEN_CURLY_CLOSE, error_context, &last)) {
            AstCodeBlock* ret = (AstCodeBlock*)malloc(sizeof(AstCodeBlock));
            ret->type = AST_CODE_BLOCK;
            ret->start = first.start;
            ret->end = first.end;
            ret->children_count = statments.count;
            ret->children = (Ast**)statments.data;
            return ret;
        }
    failed:
        for (int i = 0; i < statments.count; i++) {
            freeAst((Ast*)statments.data[i]);
        }
        freeArrayList(&statments);
        return PARSE_ERROR;
    }
    return NULL;
}

static AstParameterDefinition* parseParameterDefinition(Scanner* scanner, ErrorContext* error_context) {
    Token name;
    if (consume(scanner, TOKEN_IDENTIFIER, &name)) {
        Ast* type = NULL;
        if(expectToken(scanner, TOKEN_COLON, error_context, NULL)) {
            type = parseType(scanner, error_context);
            if (type == NULL || type == PARSE_ERROR) {
                if (type == NULL) {
                    addError(error_context, "Expected a type", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
        } else {
            goto failed;
        }
        AstParameterDefinition* ret = (AstParameterDefinition*)malloc(sizeof(AstParameterDefinition));
        ret->type = AST_PARAMETER_DEFINITION;
        ret->start = name.start;
        ret->end = type->end;
        ret->name = getStringCopyFromFile(scanner->file, name.start, name.end);
        ret->parameter_type = type;
        return ret;
    failed:
        freeAst(type);
        return PARSE_ERROR;
    }
    return NULL;
}

static Ast* parseExpressionLevelBase(Scanner* scanner, ErrorContext* error_context) {
    Token first;
    if (consume(scanner, TOKEN_INT_CONST, &first)) {
        AstIntegerLiteral* ret = (AstIntegerLiteral*)malloc(sizeof(AstIntegerLiteral));
        ret->type = AST_INTEGER_LITERAL;
        ret->start = first.start;
        ret->end = first.end;
        ret->integer_string = getStringCopyFromFile(scanner->file, first.start, first.end);
        return (Ast*)ret;
    } else if (consume(scanner, TOKEN_FLOAT_CONST, &first)) {
        AstFloatLiteral* ret = (AstFloatLiteral*)malloc(sizeof(AstFloatLiteral));
        ret->type = AST_FLOAT_LITERAL;
        ret->start = first.start;
        ret->end = first.end;
        ret->float_string = getStringCopyFromFile(scanner->file, first.start, first.end);
        return (Ast*)ret;
    } else if (consume(scanner, TOKEN_IDENTIFIER, &first)) {
        AstVariableAccess* ret = (AstVariableAccess*)malloc(sizeof(AstVariableAccess));
        ret->type = AST_VARIABLE_ACCESS;
        ret->start = first.start;
        ret->end = first.end;
        ret->name = getStringCopyFromFile(scanner->file, first.start, first.end);
        return (Ast*)ret;
    } else if(consume(scanner, TOKEN_ROUND_OPEN, &first)) {
        Ast* ret = parseExpression(scanner, error_context);
        if(ret == PARSE_ERROR || ret == NULL) {
            if(ret == NULL) {
                addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
            }
            return PARSE_ERROR;
        }
        if(!expectToken(scanner, TOKEN_ROUND_CLOSE, error_context, NULL)) {
            freeAst(ret);
            return PARSE_ERROR;
        }
        return ret;
    } else if (consume(scanner, TOKEN_STRING_CONST, &first)) {
        AstStringLiteral* ret = (AstStringLiteral*)malloc(sizeof(AstStringLiteral));
        ret->type = AST_STRING_LITERAL;
        ret->start = first.start;
        ret->end = first.end;
        ret->string_content = getStringCopyFromFile(scanner->file, first.start + 1, first.end - 1);
        int new = 0;
        int old = 0;
        while (ret->string_content[old] != 0) {
            if (ret->string_content[old] == '\\') {
                int length;
                int codepoint = parseEscapeCode(ret->string_content + old + 1, &length);
                if (codepoint == -1) {
                    addError(error_context, "Found an illegal escape code", getCurrentScannerPosition(scanner), ERROR);
                } else {
                    new += printUTF8(codepoint, ret->string_content + new);
                    old += length;
                }
                old++;
            } else {
                ret->string_content[new] = ret->string_content[old];
                new ++;
                old++;
            }
        }
        ret->string_content[new] = 0;
        return (Ast*)ret;
    } else if (consume(scanner, TOKEN_CHAR_CONST, &first)) {
        int character;
        char* content = getStringRefFromFile(scanner->file, first.start + 1);
        if (first.start + 3 == first.end) {
            character = (int)content[0];
        } else if (content[0] == '\\') {
            int length;
            character = parseEscapeCode(content + 1, &length);
            if (character == -1) {
                addError(error_context, "Found an illegal escape code", getCurrentScannerPosition(scanner), ERROR);
            } else if (first.start + 3 + length != first.end) {
                addError(error_context, "A character literal can only contain one character", getCurrentScannerPosition(scanner), ERROR);
            }
        } else {
            addError(error_context, "A character literal can only contain one character", getCurrentScannerPosition(scanner), ERROR);
            character = (int)content[0];
        }
        AstCharacterLiteral* ret = (AstCharacterLiteral*)malloc(sizeof(AstCharacterLiteral));
        ret->type = AST_CHARACTER_LITERAL;
        ret->start = first.start;
        ret->end = first.end;
        ret->character = character;
        return (Ast*)ret;
    }
    return NULL;
}

/* () [] ++ -- */
static Ast* parseExpressionLevelPostfix(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelBase(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (test(scanner, 0, TOKEN_ROUND_OPEN) || test(scanner, 0, TOKEN_SQUARE_OPEN) || test(scanner, 0, TOKEN_DBL_PLUS) || test(scanner, 0, TOKEN_DBL_MINUS)) {
            Token last;
            if (accept(scanner, TOKEN_ROUND_OPEN)) {
                ArrayList parameters;
                initArrayList(&parameters);
                Ast* parameter;
                while ((parameters.count == 0 || accept(scanner, TOKEN_COMMA)) && (parameter = parseExpression(scanner, error_context)) != NULL) {
                    if (parameter == PARSE_ERROR) {
                        for (int i = 0; i < parameters.count; i++) {
                            freeAst((Ast*)parameters.data[i]);
                        }
                        freeArrayList(&parameters);
                        goto failed;
                    }
                    pushToArrayList(&parameters, (void*)parameter);
                }
                Token last;
                if (!expectToken(scanner, TOKEN_ROUND_CLOSE, error_context, &last)) {
                    for (int i = 0; i < parameters.count; i++) {
                        freeAst((Ast*)parameters.data[i]);
                    }
                    freeArrayList(&parameters);
                    goto failed;
                }
                AstCall* parent = (AstCall*)malloc(sizeof(AstCall));
                parent->type = AST_CALL;
                parent->start = ret->start;
                parent->end = last.end;
                parent->to_call = ret;
                parent->parameter_count = parameters.count;
                parent->parameters = (Ast**)parameters.data;
                ret = (Ast*)parent;
            } else if (accept(scanner, TOKEN_SQUARE_OPEN)) {
                ArrayList parameters;
                initArrayList(&parameters);
                Ast* index = parseExpression(scanner, error_context);
                if (index == PARSE_ERROR || index == NULL) {
                    if (index == NULL) {
                        addError(error_context, "Expected a index", getCurrentScannerPosition(scanner), ERROR);
                    }
                    goto failed;
                }
                Token last;
                if (!expectToken(scanner, TOKEN_SQUARE_CLOSE, error_context, &last)) {
                    freeAst(index);
                    goto failed;
                }
                AstIndex* parent = (AstIndex*)malloc(sizeof(AstIndex));
                parent->type = AST_INDEX;
                parent->start = ret->start;
                parent->end = last.end;
                parent->pointer = ret;
                parent->index = index;
                ret = (Ast*)parent;
            } else if (consume(scanner, TOKEN_DBL_PLUS, &last)) {
                AstUnaryOperation* parent = (AstUnaryOperation*)malloc(sizeof(AstUnaryOperation));
                parent->type = AST_INCREMENT;
                parent->start = ret->start;
                parent->end = last.end;
                parent->operand = ret;
                ret = (Ast*)parent;
            } else if (consume(scanner, TOKEN_DBL_MINUS, &last)) {
                AstUnaryOperation* parent = (AstUnaryOperation*)malloc(sizeof(AstUnaryOperation));
                parent->type = AST_DECREMENT;
                parent->start = ret->start;
                parent->end = last.end;
                parent->operand = ret;
                ret = (Ast*)parent;
            }
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* - + * ~ ! & [] */
static Ast* parseExpressionLevelUnary(Scanner* scanner, ErrorContext* error_context) {
    if (test(scanner, 0, TOKEN_MINUS) || test(scanner, 0, TOKEN_PLUS) || test(scanner, 0, TOKEN_STAR) || test(scanner, 0, TOKEN_TILDE) || test(scanner, 0, TOKEN_EXCL) || test(scanner, 0, TOKEN_AND)){
        Token first;
        AstType type;
        if (consume(scanner, TOKEN_MINUS, &first)) {
            type = AST_NEGATIVE;
        } else if (consume(scanner, TOKEN_PLUS, &first)) {
            type = AST_POSITIVE;
        } else if (consume(scanner, TOKEN_STAR, &first)) {
            type = AST_REFERENCE;
        } else if (consume(scanner, TOKEN_TILDE, &first) || accept(scanner, TOKEN_EXCL)) {
            type = AST_NOT;
        } else if (consume(scanner, TOKEN_AND, &first)) {
             type = AST_DEREFERENCE;
        }
        Ast* next = parseExpressionLevelUnary(scanner, error_context);
        if (next == NULL && type == AST_REFERENCE) {
            Ast* ret = (Ast*)malloc(sizeof(Ast));
            ret->type = AST_ADDRESS;
            ret->start = first.start;
            ret->end = first.end;
            return ret;
        } else {
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                return PARSE_ERROR;
            }
            AstUnaryOperation* ret = (AstUnaryOperation*)malloc(sizeof(AstUnaryOperation));
            ret->type = type;
            ret->start = first.start;
            ret->end = next->end;
            ret->operand = next;
            return (Ast*)ret;
        }
    } else if (test(scanner, 0, TOKEN_SQUARE_OPEN)) {
        Token first;
        consume(scanner, TOKEN_SQUARE_OPEN, &first);
        Ast* number = parseExpression(scanner, error_context);
        if(number == NULL) {
            addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
        } else if(number != PARSE_ERROR) {
            if(expectToken(scanner, TOKEN_SQUARE_CLOSE, error_context, NULL)) {
                Ast* value = parseExpressionLevelUnary(scanner, error_context);
                if (number == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                } else if (number != PARSE_ERROR) {
                    AstBinaryOperation* ret = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
                    ret->type = AST_ARRAY;
                    ret->start = first.start;
                    ret->end = value->end;
                    ret->lhs = number;
                    ret->rhs = value;
                    return (Ast*)ret;
                }
            }
            freeAst(number);
        }
        return PARSE_ERROR;
    } else {
        return parseExpressionLevelPostfix(scanner, error_context);
    }
}

/* * / % */
static Ast* parseExpressionLevelMultiplicative(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelUnary(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (test(scanner, 0, TOKEN_STAR) || test(scanner, 0, TOKEN_SLASH) || test(scanner, 0, TOKEN_PERCENT)) {
            AstType type = AST_NONE;
            if (accept(scanner, TOKEN_STAR)) {
                type = AST_MULTIPLY;
            } else if (accept(scanner, TOKEN_SLASH)) {
                type = AST_DIVIDE;
            } else if (accept(scanner, TOKEN_PERCENT)) {
                type = AST_REMAINDER;
            }
            Ast* next = parseExpressionLevelUnary(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = type;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* + - */
static Ast* parseExpressionLevelAdditive(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelMultiplicative(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (test(scanner, 0, TOKEN_PLUS) || test(scanner, 0, TOKEN_MINUS)) {
            AstType type = AST_NONE;
            if (accept(scanner, TOKEN_PLUS)) {
                type = AST_ADD;
            } else if (accept(scanner, TOKEN_MINUS)) {
                type = AST_SUBTRACT;
            }
            Ast* next = parseExpressionLevelMultiplicative(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = type;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* << >> */
static Ast* parseExpressionLevelShift(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelAdditive(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (test(scanner, 0, TOKEN_DBL_LES) || test(scanner, 0, TOKEN_DBL_GTR)) {
            AstType type = AST_NONE;
            if (accept(scanner, TOKEN_DBL_LES)) {
                type = AST_SHIFT_LEFT;
            } else if (accept(scanner, TOKEN_DBL_GTR)) {
                type = AST_SHIFT_RIGHT;
            }
            Ast* next = parseExpressionLevelAdditive(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = type;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* & */
static Ast* parseExpressionLevelAnd(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelShift(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (accept(scanner, TOKEN_AND)) {
            Ast* next = parseExpressionLevelShift(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = AST_AND;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* ^ */
static Ast* parseExpressionLevelXor(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelAnd(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (accept(scanner, TOKEN_CARET)) {
            Ast* next = parseExpressionLevelAnd(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = AST_XOR;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* | */
static Ast* parseExpressionLevelOr(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelXor(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (accept(scanner, TOKEN_PIPE)) {
            Ast* next = parseExpressionLevelXor(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = AST_OR;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* > < >= <= == != */
static Ast* parseExpressionLevelRelational(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelOr(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (test(scanner, 0, TOKEN_GTR) || test(scanner, 0, TOKEN_LES) || test(scanner, 0, TOKEN_GTR_EQU) || test(scanner, 0, TOKEN_LES_EQU) || test(scanner, 0, TOKEN_DBL_EQU) || test(scanner, 0, TOKEN_EXCL_EQU)) {
            AstType type = AST_NONE;
            if (accept(scanner, TOKEN_GTR)) {
                type = AST_GREATER;
            } else if (accept(scanner, TOKEN_LES)) {
                type = AST_LESS;
            } else if (accept(scanner, TOKEN_GTR_EQU)) {
                type = AST_GREATER_EQUAL;
            } else if (accept(scanner, TOKEN_LES_EQU)) {
                type = AST_LESS_EQUAL;
            } else if (accept(scanner, TOKEN_DBL_EQU)) {
                type = AST_EQUAL;
            } else if (accept(scanner, TOKEN_EXCL_EQU)) {
                type = AST_UNEQUAL;
            }
            Ast* next = parseExpressionLevelOr(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = type;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* && */
static Ast* parseExpressionLevelLazyAnd(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelRelational(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (accept(scanner, TOKEN_DBL_AND)) {
            Ast* next = parseExpressionLevelRelational(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = AST_LAZY_AND;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

/* || */
static Ast* parseExpressionLevelLazyOr(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = parseExpressionLevelLazyAnd(scanner, error_context);
    if (ret != NULL && ret != PARSE_ERROR) {
        while (accept(scanner, TOKEN_DBL_PIPE)) {
            Ast* next = parseExpressionLevelLazyAnd(scanner, error_context);
            if (next == NULL || next == PARSE_ERROR) {
                if (next == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstBinaryOperation* parent = (AstBinaryOperation*)malloc(sizeof(AstBinaryOperation));
            parent->type = AST_LAZY_OR;
            parent->start = ret->start;
            parent->end = next->end;
            parent->lhs = ret;
            parent->rhs = next;
            ret = (Ast*)parent;
        }
        return ret;
    failed:
        freeAst(ret);
        return PARSE_ERROR;
    }
    return ret;
}

static Ast* parseExpression(Scanner* scanner, ErrorContext* error_context) { return parseExpressionLevelLazyOr(scanner, error_context); }

static Ast* parseGlobalFunction(Scanner* scanner, ErrorContext* error_context) {
    if ((test(scanner, 0, TOKEN_IDENTIFIER) && test(scanner, 1, TOKEN_ROUND_OPEN)) ||
        (test(scanner, 0, TOKEN_EXTERN) && test(scanner, 1, TOKEN_IDENTIFIER) && test(scanner, 2, TOKEN_ROUND_OPEN))) {
        Token first;
        Token last;
        Ast* return_type = NULL;
        AstCodeBlock* body = NULL;
        AstParameterDefinition* parameter;
        ArrayList parameters;
        initArrayList(&parameters);
        bool is_extern = consume(scanner, TOKEN_EXTERN, &first);
        Token name;
        consume(scanner, TOKEN_IDENTIFIER, &name);
        accept(scanner, TOKEN_ROUND_OPEN);
        while ((parameters.count == 0 || accept(scanner, TOKEN_COMMA)) && (parameter = parseParameterDefinition(scanner, error_context)) != NULL) {
            if (parameter == PARSE_ERROR) {
                goto failed;
            }
            pushToArrayList(&parameters, (void*)parameter);
        }
        bool is_vararg = accept(scanner, TOKEN_DBL_POINT);
        if (!expectToken(scanner, TOKEN_ROUND_CLOSE, error_context, NULL)) {
            goto failed;
        }
        return_type = parseType(scanner, error_context);
        if (return_type == PARSE_ERROR) {
            goto failed;
        }
        if (!is_extern) {
            body = parseCodeBlock(scanner, error_context);
            if (body == NULL || body == PARSE_ERROR) {
                if (body == NULL) {
                    addError(error_context, "Expected a function body", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
        } else {
            if (!expectToken(scanner, TOKEN_SEMICOLON, error_context, &last)) {
                goto failed;
            }
        }
        AstFunctionDefinition* ret = (AstFunctionDefinition*)malloc(sizeof(AstFunctionDefinition));
        ret->type = AST_FUNCTION_DEFINITION;
        ret->start = first.start;
        if (is_extern) {
            ret->end = last.end;
        } else {
            ret->end = body->end;
        }
        ret->is_extern = is_extern;
        ret->is_vararg = is_vararg;
        ret->name = getStringCopyFromFile(scanner->file, name.start, name.end);
        ret->parameter_count = parameters.count;
        ret->parameters = (AstParameterDefinition**)parameters.data;
        ret->return_type = return_type;
        ret->body = body;
        return (Ast*)ret;
    failed:
        for (int i = 0; i < parameters.count; i++) {
            freeAst((Ast*)parameters.data[i]);
        }
        freeArrayList(&parameters);
        freeAst(return_type);
        freeAst((Ast*)body);
        return PARSE_ERROR;
    }
    return NULL;
}

static Ast* parseGlobalVariable(Scanner* scanner, ErrorContext* error_context) {
    if(test(scanner, 0, TOKEN_IDENTIFIER) || (test(scanner, 0, TOKEN_EXTERN) && test(scanner, 1, TOKEN_IDENTIFIER))) {
        Token first;
        bool is_extern = consume(scanner, TOKEN_EXTERN, &first); 
        Token name;
        consume(scanner, TOKEN_IDENTIFIER, &name);
        Token error;
        if(accept(scanner, TOKEN_COLON)) {
            Ast* type = parseType(scanner, error_context);
            if(type == PARSE_ERROR || type == NULL) {
                if(type == NULL) {
                    addError(error_context, "Expected a type", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            if (accept(scanner, TOKEN_EQU)) {
                if (is_extern) {
                    addError(error_context, "Extern variables can't be initialized", first.start, ERROR);
                    freeAst((Ast*)type);
                    goto failed;
                }
                Ast* value = parseExpression(scanner, error_context);
                if (value == PARSE_ERROR || value == NULL) {
                    if (value == NULL) {
                        addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                    }
                    freeAst((Ast*)type);
                    goto failed;
                }
                AstVariableDefinition* ret = (AstVariableDefinition*)malloc(sizeof(AstVariableDefinition));
                ret->type = AST_VARIABLE_DEFINITION;
                ret->start = first.start;
                ret->end = value->end;
                ret->is_extern = is_extern;
                ret->name = getStringCopyFromFile(scanner->file, name.start, name.end);
                ret->variable_type = type;
                ret->initial_value = value;
                return (Ast*)ret;
            } else {
                AstVariableDefinition* ret = (AstVariableDefinition*)malloc(sizeof(AstVariableDefinition));
                ret->type = AST_VARIABLE_DEFINITION;
                ret->start = first.start;
                ret->end = type->end;
                ret->is_extern = is_extern;
                ret->name = getStringCopyFromFile(scanner->file, name.start, name.end);
                ret->variable_type = type;
                ret->initial_value = NULL;
                return (Ast*)ret;
            }
        } else if(consume(scanner, TOKEN_COLON_EQU, &error)) {
            if (is_extern) {
                addError(error_context, "Extern variables can't be initialized", first.start, ERROR);
                goto failed;
            }
            Ast* value = parseExpression(scanner, error_context);
            if (value == PARSE_ERROR || value == NULL) {
                if (value == NULL) {
                    addError(error_context, "Expected an expression", getCurrentScannerPosition(scanner), ERROR);
                }
                goto failed;
            }
            AstVariableDefinition* ret = (AstVariableDefinition*)malloc(sizeof(AstVariableDefinition));
            ret->type = AST_VARIABLE_DEFINITION;
            ret->start = first.start;
            ret->end = value->end;
            ret->is_extern = is_extern;
            ret->name = getStringCopyFromFile(scanner->file, name.start, name.end);
            ret->variable_type = NULL;
            ret->initial_value = value;
            return (Ast*)ret;
        } else {
            addErrorf(error_context, error.start, ERROR, "Expected a '%s', '%s' or '%s' but found a '%s'",
                getTokenName(TOKEN_CURLY_OPEN), getTokenName(TOKEN_COLON), getTokenName(TOKEN_COLON_EQU), getTokenName(error.type));
            goto failed;
        }
    failed:
        return PARSE_ERROR;
    }
    return NULL;
}

static Ast* parseRootElement(Scanner* scanner, ErrorContext* error_context) {
    Ast* ret = NULL;
    (ret = (Ast*)parseGlobalFunction(scanner, error_context)) != NULL ||
    (ret = (Ast*)parseGlobalVariable(scanner, error_context)) != NULL;
    return ret;
}

static AstRoot* parseRoot(Scanner* scanner, ErrorContext* error_context) {
    ArrayList list;
    initArrayList(&list);
    Ast* tmp;
    while ((tmp = parseRootElement(scanner, error_context)) != NULL) {
        if (tmp == PARSE_ERROR) {
            goto failed;
        }
        pushToArrayList(&list, tmp);
        while (accept(scanner, TOKEN_SEMICOLON))
            ;
    }
    AstRoot* ret = (AstRoot*)malloc(sizeof(AstRoot));
    ret->type = AST_ROOT;
    ret->children_count = list.count;
    ret->children = (Ast**)list.data;
    if (ret->children_count > 0) {
        ret->start = ret->children[0]->start;
        ret->end = ret->children[ret->children_count - 1]->end;
    } else {
        ret->start = NOPOS;
        ret->end = NOPOS;
    }
    return ret;
failed:
    for (int i = 0; i < list.count; i++) {
        freeAst((Ast*)list.data[i]);
    }
    freeArrayList(&list);
    return PARSE_ERROR;
}

Ast* parseFile(File* file, ErrorContext* error_context) {
    if(file->ast == NULL) {
        Scanner scanner;
        initScanner(&scanner, file);
        file->ast = (Ast*)parseRoot(&scanner, error_context);
        if(file->ast != PARSE_ERROR) {
            expectToken(&scanner, TOKEN_EOF, error_context, NULL);
        }
        freeScanner(&scanner);
    }
    return file->ast;
}
