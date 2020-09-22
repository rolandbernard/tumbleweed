
#include <stdlib.h>

#include "parser/ast.h"

static void freeSimpleAst(Ast* ast) { free(ast); }

static void freeAstRoot(AstRoot* ast) {
    if (ast != NULL) {
        for (int i = 0; i < ast->children_count; i++) {
            freeAst(ast->children[i]);
        }
        ast->children_count = 0;
        free(ast->children);
        ast->children = NULL;
        free(ast);
    }
}

static void freeAstParameterDefinition(AstParameterDefinition* ast) {
    if (ast != NULL) {
        free(ast->name);
        ast->name = NULL;
        freeAst(ast->parameter_type);
        ast->parameter_type = NULL;
        free(ast);
    }
}

static void freeAstCodeBlock(AstCodeBlock* ast) {
    if (ast != NULL) {
        for (int i = 0; i < ast->children_count; i++) {
            freeAst(ast->children[i]);
        }
        ast->children_count = 0;
        free(ast->children);
        ast->children = NULL;
        free(ast);
    }
}

static void freeAstFunctionDefinition(AstFunctionDefinition* ast) {
    if (ast != NULL) {
        free(ast->name);
        ast->name = NULL;
        for (int i = 0; i < ast->parameter_count; i++) {
            freeAstParameterDefinition(ast->parameters[i]);
        }
        ast->parameter_count = 0;
        free(ast->parameters);
        ast->parameters = NULL;
        freeAst(ast->return_type);
        ast->return_type = NULL;
        freeAstCodeBlock(ast->body);
        ast->body = NULL;
        free(ast);
    }
}

static void freeAstVariableDefinition(AstVariableDefinition* ast) {
    if (ast != NULL) {
        free(ast->name);
        ast->name = NULL;
        freeAst(ast->variable_type);
        ast->variable_type = NULL;
        freeAst(ast->initial_value);
        ast->initial_value = NULL;
        free(ast);
    }
}

static void freeAstAssignment(AstAssignment* ast) {
    if (ast != NULL) {
        freeAst(ast->destination);
        ast->destination = NULL;
        freeAst(ast->value);
        ast->value = NULL;
        free(ast);
    }
}

static void freeAstIfElse(AstIfElse* ast) {
    if (ast != NULL) {
        freeAst(ast->condition);
        ast->condition = NULL;
        freeAst(ast->if_block);
        ast->if_block = NULL;
        freeAst(ast->else_block);
        ast->else_block = NULL;
        free(ast);
    }
}

static void freeAstForLoop(AstForLoop* ast) {
    if (ast != NULL) {
        freeAst(ast->condition);
        ast->condition = NULL;
        freeAst(ast->code_block);
        ast->code_block = NULL;
        free(ast);
    }
}

static void freeAstIntegerLiteral(AstIntegerLiteral* ast) {
    if (ast != NULL) {
        free(ast->integer_string);
        ast->integer_string = NULL;
        free(ast);
    }
}

static void freeAstFloatLiteral(AstFloatLiteral* ast) {
    if (ast != NULL) {
        free(ast->float_string);
        ast->float_string = NULL;
        free(ast);
    }
}

static void freeAstStringLiteral(AstStringLiteral* ast) {
    if (ast != NULL) {
        free(ast->string_content);
        ast->string_content = NULL;
        free(ast);
    }
}

static void freeAstCharacterLiteral(AstCharacterLiteral* ast) {
    if (ast != NULL) {
        free(ast);
    }
}

static void freeAstBinaryOperation(AstBinaryOperation* ast) {
    if (ast != NULL) {
        freeAst(ast->lhs);
        ast->lhs = NULL;
        freeAst(ast->rhs);
        ast->rhs = NULL;
        free(ast);
    }
}

static void freeAstUnaryOperation(AstUnaryOperation* ast) {
    if (ast != NULL) {
        freeAst(ast->operand);
        ast->operand = NULL;
        free(ast);
    }
}

static void freeAstVariableAccess(AstVariableAccess* ast) {
    if (ast != NULL) {
        free(ast->name);
        ast->name = NULL;
        free(ast);
    }
}

static void freeAstCall(AstCall* ast) {
    if (ast != NULL) {
        freeAst(ast->to_call);
        ast->to_call = NULL;
        for (int i = 0; i < ast->parameter_count; i++) {
            freeAst(ast->parameters[i]);
        }
        ast->parameter_count = 0;
        free(ast->parameters);
        ast->parameters = NULL;
        free(ast);
    }
}

static void freeAstIndex(AstIndex* ast) {
    if (ast != NULL) {
        freeAst(ast->pointer);
        ast->pointer = NULL;
        freeAst(ast->index);
        ast->index = NULL;
        free(ast);
    }
}

typedef void (*FreeFunction)(Ast*);

static FreeFunction free_functions[] = {
    [AST_ROOT] = (FreeFunction)freeAstRoot,
    [AST_CODE_BLOCK] = (FreeFunction)freeAstCodeBlock,
    [AST_PARAMETER_DEFINITION] = (FreeFunction)freeAstParameterDefinition,
    [AST_FUNCTION_DEFINITION] = (FreeFunction)freeAstFunctionDefinition,
    [AST_VARIABLE_DEFINITION] = (FreeFunction)freeAstVariableDefinition,
    [AST_IF_ELSE] = (FreeFunction)freeAstIfElse,
    [AST_FOR_LOOP] = (FreeFunction)freeAstForLoop,
    [AST_INTEGER_LITERAL] = (FreeFunction)freeAstIntegerLiteral,
    [AST_FLOAT_LITERAL] = (FreeFunction)freeAstFloatLiteral,
    [AST_STRING_LITERAL] = (FreeFunction)freeAstStringLiteral,
    [AST_CHARACTER_LITERAL] = (FreeFunction)freeAstCharacterLiteral,
    [AST_VARIABLE_ACCESS] = (FreeFunction)freeAstVariableAccess,
    [AST_CALL] = (FreeFunction)freeAstCall,
    [AST_INDEX] = (FreeFunction)freeAstIndex,

    // Assignments
    [AST_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_OR_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_AND_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_XOR_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_SHIFT_RIGHT_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_SHIFT_LEFT_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_ADD_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_SUBTRACT_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_MULTIPLY_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_DIVIDE_ASSIGNMENT] = (FreeFunction)freeAstAssignment,
    [AST_REMAINDER_ASSIGNMENT] = (FreeFunction)freeAstAssignment,

    // Binary operation
    [AST_LAZY_AND] = (FreeFunction)freeAstBinaryOperation,
    [AST_LAZY_OR] = (FreeFunction)freeAstBinaryOperation,
    [AST_EQUAL] = (FreeFunction)freeAstBinaryOperation,
    [AST_UNEQUAL] = (FreeFunction)freeAstBinaryOperation,
    [AST_GREATER_EQUAL] = (FreeFunction)freeAstBinaryOperation,
    [AST_LESS_EQUAL] = (FreeFunction)freeAstBinaryOperation,
    [AST_GREATER] = (FreeFunction)freeAstBinaryOperation,
    [AST_LESS] = (FreeFunction)freeAstBinaryOperation,
    [AST_OR] = (FreeFunction)freeAstBinaryOperation,
    [AST_AND] = (FreeFunction)freeAstBinaryOperation,
    [AST_XOR] = (FreeFunction)freeAstBinaryOperation,
    [AST_SHIFT_RIGHT] = (FreeFunction)freeAstBinaryOperation,
    [AST_SHIFT_LEFT] = (FreeFunction)freeAstBinaryOperation,
    [AST_ADD] = (FreeFunction)freeAstBinaryOperation,
    [AST_SUBTRACT] = (FreeFunction)freeAstBinaryOperation,
    [AST_MULTIPLY] = (FreeFunction)freeAstBinaryOperation,
    [AST_DIVIDE] = (FreeFunction)freeAstBinaryOperation,
    [AST_REMAINDER] = (FreeFunction)freeAstBinaryOperation,
    [AST_ARRAY] = (FreeFunction)freeAstBinaryOperation,

    // Unary operation
    [AST_RETURN] = (FreeFunction)freeAstUnaryOperation,
    [AST_DEREFERENCE] = (FreeFunction)freeAstUnaryOperation,
    [AST_POSITIVE] = (FreeFunction)freeAstUnaryOperation,
    [AST_NEGATIVE] = (FreeFunction)freeAstUnaryOperation,
    [AST_REFERENCE] = (FreeFunction)freeAstUnaryOperation,
    [AST_NOT] = (FreeFunction)freeAstUnaryOperation,
    [AST_INCREMENT] = (FreeFunction)freeAstUnaryOperation,
    [AST_DECREMENT] = (FreeFunction)freeAstUnaryOperation,

    // Simple Ast
    [AST_ADDRESS] = (FreeFunction)freeSimpleAst,
};

void freeAst(Ast* node) {
    if (node != NULL && node != PARSE_ERROR) {
        if (free_functions[node->type] != NULL) {
            free_functions[node->type](node);
        }
    }
}
