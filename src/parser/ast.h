#ifndef _AST_H_
#define _AST_H_

#include "common/util.h"

#define AST_BASE                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
    AstType type;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    Position start;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    Position end;

typedef enum {
    AST_NONE,
    
    AST_ROOT,
    AST_CODE_BLOCK,
    AST_PARAMETER_DEFINITION,
    AST_FUNCTION_DEFINITION,
    AST_VARIABLE_DEFINITION,
    AST_IF_ELSE,
    AST_FOR_LOOP,
    AST_INTEGER_LITERAL,
    AST_FLOAT_LITERAL,
    AST_STRING_LITERAL,
    AST_CHARACTER_LITERAL,
    AST_VARIABLE_ACCESS,
    AST_CALL,
    AST_INDEX,
    
    // Assignments
    AST_ASSIGNMENT,
    AST_OR_ASSIGNMENT,
    AST_AND_ASSIGNMENT,
    AST_XOR_ASSIGNMENT,
    AST_SHIFT_RIGHT_ASSIGNMENT,
    AST_SHIFT_LEFT_ASSIGNMENT,
    AST_ADD_ASSIGNMENT,
    AST_SUBTRACT_ASSIGNMENT,
    AST_MULTIPLY_ASSIGNMENT,
    AST_DIVIDE_ASSIGNMENT,
    AST_REMAINDER_ASSIGNMENT,
    
    // Binary operation
    AST_LAZY_AND,
    AST_LAZY_OR,
    AST_EQUAL,
    AST_UNEQUAL,
    AST_GREATER_EQUAL,
    AST_LESS_EQUAL,
    AST_GREATER,
    AST_LESS,
    AST_OR,
    AST_AND,
    AST_XOR,
    AST_SHIFT_RIGHT,
    AST_SHIFT_LEFT,
    AST_ADD,
    AST_SUBTRACT,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_REMAINDER,
    AST_ARRAY,
    
    // Unary operation 
    AST_RETURN,
    AST_DEREFERENCE,
    AST_POSITIVE,
    AST_NEGATIVE,
    AST_REFERENCE,
    AST_NOT,
    AST_BOOL_NOT,
    AST_INCREMENT,
    AST_DECREMENT,
    
    // Simple Ast
    AST_ADDRESS,
} AstType;

typedef struct {
    AST_BASE
} Ast;

typedef struct {
    AST_BASE
    int children_count;
    Ast** children;
} AstRoot;

typedef struct {
    AST_BASE
    int children_count;
    Ast** children;
} AstCodeBlock;

typedef struct {
    AST_BASE
    char* name;
    Ast* parameter_type;
} AstParameterDefinition;

typedef struct {
    AST_BASE
    bool is_extern;
    bool is_vararg;
    char* name;
    int parameter_count;
    AstParameterDefinition** parameters;
    Ast* return_type;
    AstCodeBlock* body;
} AstFunctionDefinition;

typedef struct {
    AST_BASE
    bool is_extern;
    char* name;
    Ast* variable_type;
    Ast* initial_value;
} AstVariableDefinition;

typedef struct {
    AST_BASE
    Ast* destination;
    Ast* value;
} AstAssignment;

typedef struct {
    AST_BASE
    Ast* condition;
    Ast* if_block;
    Ast* else_block;
} AstIfElse;

typedef struct {
    AST_BASE
    Ast* condition;
    Ast* code_block;
} AstForLoop;

typedef struct {
    AST_BASE
    char* integer_string;
} AstIntegerLiteral;

typedef struct {
    AST_BASE
    char* float_string;
} AstFloatLiteral;

typedef struct {
    AST_BASE
    char* string_content;
} AstStringLiteral;

typedef struct {
    AST_BASE
    int character;
} AstCharacterLiteral;

typedef struct {
    AST_BASE
    Ast* lhs;
    Ast* rhs;
} AstBinaryOperation;

typedef struct {
    AST_BASE
    Ast* operand;
} AstUnaryOperation;

typedef struct {
    AST_BASE
    char* name;
} AstVariableAccess;

typedef struct {
    AST_BASE
    Ast* to_call;
    int parameter_count;
    Ast** parameters;
} AstCall;

typedef struct {
    AST_BASE
    Ast* pointer;
    Ast* index;
} AstIndex;

void freeAst(Ast* node);

#endif
