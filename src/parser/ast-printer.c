
#include <stdio.h>

#include "parser/ast-printer.h"
#include "parser/ast.h"

static const char* ast_type_names[] = {
    [AST_ROOT] = "AST_ROOT",
    [AST_CODE_BLOCK] = "AST_CODE_BLOCK",
    [AST_PARAMETER_DEFINITION] = "AST_PARAMETER_DEFINITION",
    [AST_FUNCTION_DEFINITION] = "AST_FUNCTION_DEFINITION",
    [AST_VARIABLE_DEFINITION] = "AST_VARIABLE_DEFINITION",
    [AST_IF_ELSE] = "AST_IF_ELSE",
    [AST_FOR_LOOP] = "AST_FOR_LOOP",
    [AST_INTEGER_LITERAL] = "AST_INTEGER_LITERAL",
    [AST_FLOAT_LITERAL] = "AST_FLOAT_LITERAL",
    [AST_VARIABLE_ACCESS] = "AST_VARIABLE_ACCESS",
    [AST_CALL] = "AST_CALL",
    [AST_INDEX] = "AST_INDEX",

    // Assignments
    [AST_ASSIGNMENT] = "AST_ASSIGNMENT",
    [AST_OR_ASSIGNMENT] = "AST_OR_ASSIGNMENT",
    [AST_AND_ASSIGNMENT] = "AST_AND_ASSIGNMENT",
    [AST_XOR_ASSIGNMENT] = "AST_XOR_ASSIGNMENT",
    [AST_SHIFT_RIGHT_ASSIGNMENT] = "AST_SHIFT_RIGHT_ASSIGNMENT",
    [AST_SHIFT_LEFT_ASSIGNMENT] = "AST_SHIFT_LEFT_ASSIGNMENT",
    [AST_ADD_ASSIGNMENT] = "AST_ADD_ASSIGNMENT",
    [AST_SUBTRACT_ASSIGNMENT] = "AST_SUBTRACT_ASSIGNMENT",
    [AST_MULTIPLY_ASSIGNMENT] = "AST_MULTIPLY_ASSIGNMENT",
    [AST_DIVIDE_ASSIGNMENT] = "AST_DIVIDE_ASSIGNMENT",
    [AST_REMAINDER_ASSIGNMENT] = "AST_REMAINDER_ASSIGNMENT",

    // Binary operation
    [AST_LAZY_AND] = "AST_LAZY_AND",
    [AST_LAZY_OR] = "AST_LAZY_OR",
    [AST_EQUAL] = "AST_EQUAL",
    [AST_UNEQUAL] = "AST_UNEQUAL",
    [AST_GREATER_EQUAL] = "AST_GREATER_EQUAL",
    [AST_LESS_EQUAL] = "AST_LESS_EQUAL",
    [AST_GREATER] = "AST_GREATER",
    [AST_LESS] = "AST_LESS",
    [AST_OR] = "AST_OR",
    [AST_AND] = "AST_AND",
    [AST_XOR] = "AST_XOR",
    [AST_SHIFT_RIGHT] = "AST_SHIFT_RIGHT",
    [AST_SHIFT_LEFT] = "AST_SHIFT_LEFT",
    [AST_ADD] = "AST_ADD",
    [AST_SUBTRACT] = "AST_SUBTRACT",
    [AST_MULTIPLY] = "AST_MULTIPLY",
    [AST_DIVIDE] = "AST_DIVIDE",
    [AST_REMAINDER] = "AST_REMAINDER",
    [AST_ARRAY] = "AST_ARRAY",

    // Unary operation
    [AST_RETURN] = "AST_RETURN",
    [AST_DEREFERENCE] = "AST_DEREFERENCE",
    [AST_POSITIVE] = "AST_POSITIVE",
    [AST_NEGATIVE] = "AST_NEGATIVE",
    [AST_REFERENCE] = "AST_REFERENCE",
    [AST_NOT] = "AST_NOT",
    [AST_INCREMENT] = "AST_INCREMENT",
    [AST_DECREMENT] = "AST_DECREMENT",

    // Simple Ast
    [AST_ADDRESS] = "AST_ADDRESS",
};

static void fillIndentation(char* indentation, int indent) {
    for(int i = 0; i < indent; i++) {
        indentation[i] = ' ';
    }
    indentation[indent] = 0;
}

static void printSimpleAst(FILE* file, Ast* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAst\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
}

static void printAstRoot(FILE* file, AstRoot* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstRoot\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .children:\n", indentation);
    for(int i = 0; i < ast->children_count; i++) {
        printAst(file, ast->children[i], indent + 3);
    }
}

static void printAstParameterDefinition(FILE* file, AstParameterDefinition* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstParameterDefinition\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .name:  %s\n", indentation, ast->name);
    fprintf(file, "%s .parameter_type:\n", indentation);
    printAst(file, ast->parameter_type, indent + 3);
}

static void printAstCodeBlock(FILE* file, AstCodeBlock* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstCodeBlock\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .children:\n", indentation);
    for(int i = 0; i < ast->children_count; i++) {
        printAst(file, ast->children[i], indent + 3);
    }
}

static void printAstFunctionDefinition(FILE* file, AstFunctionDefinition* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstFunctionDefinition\n", indentation);
    fprintf(file, "%s .type:     %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start:    %i\n", indentation, ast->start);
    fprintf(file, "%s .end:      %i\n", indentation, ast->end);
    fprintf(file, "%s .extern:   %i\n", indentation, ast->is_extern);
    fprintf(file, "%s .name:     %s\n", indentation, ast->name);
    fprintf(file, "%s .parameters:\n", indentation);
    for(int i = 0; i < ast->parameter_count; i++) {
        printAst(file, (Ast*)ast->parameters[i], indent + 3);
    }
    fprintf(file, "%s .return_type:\n", indentation);
    printAst(file, ast->return_type, indent + 3);
    fprintf(file, "%s .body:\n", indentation);
    printAst(file, (Ast*)ast->body, indent + 3);
}

static void printAstVariableDefinition(FILE* file, AstVariableDefinition* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstVariableDefinition\n", indentation);
    fprintf(file, "%s .type:     %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start:    %i\n", indentation, ast->start);
    fprintf(file, "%s .end:      %i\n", indentation, ast->end);
    fprintf(file, "%s .extern:   %i\n", indentation, ast->is_extern);
    fprintf(file, "%s .name:     %s\n", indentation, ast->name);
    fprintf(file, "%s .type:\n", indentation);
    printAst(file, ast->variable_type, indent + 3);
    fprintf(file, "%s .value:\n", indentation);
    printAst(file, ast->initial_value, indent + 3);
}

static void printAstAssignment(FILE* file, AstAssignment* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstAssignment\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .destination:\n", indentation);
    printAst(file, ast->destination, indent + 3);
    fprintf(file, "%s .value:\n", indentation);
    printAst(file, ast->value, indent + 3);
}

static void printAstIfElse(FILE* file, AstIfElse* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstIfElse\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .condition:\n", indentation);
    printAst(file, (Ast*)ast->condition, indent + 3);
    fprintf(file, "%s .if_block:\n", indentation);
    printAst(file, (Ast*)ast->if_block, indent + 3);
    fprintf(file, "%s .else_block:\n", indentation);
    printAst(file, (Ast*)ast->else_block, indent + 3);
}

static void printAstForLoop(FILE* file, AstForLoop* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstForLoop\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .condition:\n", indentation);
    printAst(file, (Ast*)ast->condition, indent + 3);
    fprintf(file, "%s .code_block:\n", indentation);
    printAst(file, (Ast*)ast->code_block, indent + 3);
}

static void printAstIntegerLiteral(FILE* file, AstIntegerLiteral* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstIntegerLiteral\n", indentation);
    fprintf(file, "%s .type:    %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start:   %i\n", indentation, ast->start);
    fprintf(file, "%s .end:     %i\n", indentation, ast->end);
    fprintf(file, "%s .integer: %s\n", indentation, ast->integer_string);
}

static void printAstFloatLiteral(FILE* file, AstFloatLiteral* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstFloatLiteral\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .float: %s\n", indentation, ast->float_string);
}

static void printAstBinaryOperation(FILE* file, AstBinaryOperation* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstBinaryOperation\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .lhs:\n", indentation);
    printAst(file, ast->lhs, indent + 3);
    fprintf(file, "%s .rhs:\n", indentation);
    printAst(file, ast->rhs, indent + 3);
}

static void printAstUnaryOperation(FILE* file, AstUnaryOperation* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstUnaryOperation\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .operand:\n", indentation);
    printAst(file, ast->operand, indent + 3);
}

static void printAstVariableAccess(FILE* file, AstVariableAccess* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstVariableAccess\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .name:  %s\n", indentation, ast->name);
}

static void printAstCall(FILE* file, AstCall* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstCall\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .to_call:\n", indentation);
    printAst(file, ast->to_call, indent + 3);
    fprintf(file, "%s .parameters:\n", indentation);
    for(int i = 0; i < ast->parameter_count; i++) {
        printAst(file, ast->parameters[i], indent + 3);
    }
}

static void printAstIndex(FILE* file, AstIndex* ast, int indent) {
    char indentation[indent + 1];
    fillIndentation(indentation, indent);
    fprintf(file, "%sAstCall\n", indentation);
    fprintf(file, "%s .type:  %s\n", indentation, ast_type_names[ast->type]);
    fprintf(file, "%s .start: %i\n", indentation, ast->start);
    fprintf(file, "%s .end:   %i\n", indentation, ast->end);
    fprintf(file, "%s .pointer:\n", indentation);
    printAst(file, ast->pointer, indent + 3);
    fprintf(file, "%s .index:\n", indentation);
    printAst(file, ast->index, indent + 3);
}

typedef void (*PrintFunction)(FILE*, Ast*, int);

static PrintFunction print_functions[] = {
    [AST_ROOT] = (PrintFunction)printAstRoot,
    [AST_CODE_BLOCK] = (PrintFunction)printAstCodeBlock,
    [AST_PARAMETER_DEFINITION] = (PrintFunction)printAstParameterDefinition,
    [AST_FUNCTION_DEFINITION] = (PrintFunction)printAstFunctionDefinition,
    [AST_VARIABLE_DEFINITION] = (PrintFunction)printAstVariableDefinition,
    [AST_IF_ELSE] = (PrintFunction)printAstIfElse,
    [AST_FOR_LOOP] = (PrintFunction)printAstForLoop,
    [AST_INTEGER_LITERAL] = (PrintFunction)printAstIntegerLiteral,
    [AST_FLOAT_LITERAL] = (PrintFunction)printAstFloatLiteral,
    [AST_VARIABLE_ACCESS] = (PrintFunction)printAstVariableAccess,
    [AST_CALL] = (PrintFunction)printAstCall,
    [AST_INDEX] = (PrintFunction)printAstIndex,

    // Assignments
    [AST_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_OR_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_AND_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_XOR_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_SHIFT_RIGHT_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_SHIFT_LEFT_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_ADD_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_SUBTRACT_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_MULTIPLY_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_DIVIDE_ASSIGNMENT] = (PrintFunction)printAstAssignment,
    [AST_REMAINDER_ASSIGNMENT] = (PrintFunction)printAstAssignment,

    // Binary operation
    [AST_LAZY_AND] = (PrintFunction)printAstBinaryOperation,
    [AST_LAZY_OR] = (PrintFunction)printAstBinaryOperation,
    [AST_EQUAL] = (PrintFunction)printAstBinaryOperation,
    [AST_UNEQUAL] = (PrintFunction)printAstBinaryOperation,
    [AST_GREATER_EQUAL] = (PrintFunction)printAstBinaryOperation,
    [AST_LESS_EQUAL] = (PrintFunction)printAstBinaryOperation,
    [AST_GREATER] = (PrintFunction)printAstBinaryOperation,
    [AST_LESS] = (PrintFunction)printAstBinaryOperation,
    [AST_OR] = (PrintFunction)printAstBinaryOperation,
    [AST_AND] = (PrintFunction)printAstBinaryOperation,
    [AST_XOR] = (PrintFunction)printAstBinaryOperation,
    [AST_SHIFT_RIGHT] = (PrintFunction)printAstBinaryOperation,
    [AST_SHIFT_LEFT] = (PrintFunction)printAstBinaryOperation,
    [AST_ADD] = (PrintFunction)printAstBinaryOperation,
    [AST_SUBTRACT] = (PrintFunction)printAstBinaryOperation,
    [AST_MULTIPLY] = (PrintFunction)printAstBinaryOperation,
    [AST_DIVIDE] = (PrintFunction)printAstBinaryOperation,
    [AST_REMAINDER] = (PrintFunction)printAstBinaryOperation,
    [AST_ARRAY] = (PrintFunction)printAstBinaryOperation,

    // Unary operation
    [AST_RETURN] = (PrintFunction)printAstUnaryOperation,
    [AST_DEREFERENCE] = (PrintFunction)printAstUnaryOperation,
    [AST_POSITIVE] = (PrintFunction)printAstUnaryOperation,
    [AST_NEGATIVE] = (PrintFunction)printAstUnaryOperation,
    [AST_REFERENCE] = (PrintFunction)printAstUnaryOperation,
    [AST_NOT] = (PrintFunction)printAstUnaryOperation,
    [AST_BOOL_NOT] = (PrintFunction)printAstUnaryOperation,
    [AST_INCREMENT] = (PrintFunction)printAstUnaryOperation,
    [AST_DECREMENT] = (PrintFunction)printAstUnaryOperation,

    // Simple Ast
    [AST_ADDRESS] = (PrintFunction)printSimpleAst,
};

void printAst(FILE* file, Ast* node, int indent) {
    if (node != NULL && node != PARSE_ERROR) {
        if (print_functions[node->type] != NULL) {
            print_functions[node->type](file, node, indent);
        }
    }
}
