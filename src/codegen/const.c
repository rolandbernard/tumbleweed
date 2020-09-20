
#include <string.h>

#include "codegen/const.h"

LLVMValueRef generateConstLazyAnd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstLazyOr(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstUnequal(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstGreaterEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstLessEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstGreater(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstLess(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstOr(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstAnd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstXor(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstShiftRight(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstShiftLeft(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstAdd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstSubtract(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstMultiply(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstDivide(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstRemainder(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstReference(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstDereference(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstPositive(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstNegative(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateConstNot(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}
LLVMValueRef generateConstIntegerLiteral(AstIntegerLiteral* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    int base = 10;
    char* str = ast->integer_string;
    int len = strlen(str);
    if(len > 2) {
        if(strncmp(str, "0b", 2) == 0) {
            base = 2;
            str += 2;
            len -= 2;
        } else if(strncmp(str, "0o", 2) == 0) {
            base = 8;
            str += 2;
            len -= 2;
        } else if(strncmp(str, "0x", 2) == 0) {
            base = 16;
            str += 2;
            len -= 2;
        }
    }
    return LLVMConstIntOfStringAndSize(LLVMIntType(64), str, len, base);
}

LLVMValueRef generateConstFloatLiteral(AstFloatLiteral* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return LLVMConstRealOfString(LLVMDoubleType(), ast->float_string);
}

typedef LLVMValueRef (*GenerateConstFunction)(Ast* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

static GenerateConstFunction generation_functions[] = {
    [AST_ROOT] = (GenerateConstFunction)NULL,
    [AST_CODE_BLOCK] = (GenerateConstFunction)NULL,
    [AST_PARAMETER_DEFINITION] = (GenerateConstFunction)NULL,
    [AST_FUNCTION_DEFINITION] = (GenerateConstFunction)NULL,
    [AST_VARIABLE_DEFINITION] = (GenerateConstFunction)NULL,
    [AST_IF_ELSE] = (GenerateConstFunction)NULL,
    [AST_FOR_LOOP] = (GenerateConstFunction)NULL,
    [AST_INTEGER_LITERAL] = (GenerateConstFunction)generateConstIntegerLiteral,
    [AST_FLOAT_LITERAL] = (GenerateConstFunction)generateConstFloatLiteral,
    [AST_VARIABLE_ACCESS] = (GenerateConstFunction)NULL,
    [AST_CALL] = (GenerateConstFunction)NULL,
    [AST_INDEX] = (GenerateConstFunction)NULL,

    // Assignments
    [AST_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_OR_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_AND_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_XOR_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_SHIFT_RIGHT_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_SHIFT_LEFT_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_ADD_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_SUBTRACT_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_MULTIPLY_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_DIVIDE_ASSIGNMENT] = (GenerateConstFunction)NULL,
    [AST_REMAINDER_ASSIGNMENT] = (GenerateConstFunction)NULL,

    // Binary operation
    [AST_LAZY_AND] = (GenerateConstFunction)generateConstLazyAnd,
    [AST_LAZY_OR] = (GenerateConstFunction)generateConstLazyOr,
    [AST_EQUAL] = (GenerateConstFunction)generateConstEqual,
    [AST_UNEQUAL] = (GenerateConstFunction)generateConstUnequal,
    [AST_GREATER_EQUAL] = (GenerateConstFunction)generateConstGreaterEqual,
    [AST_LESS_EQUAL] = (GenerateConstFunction)generateConstLessEqual,
    [AST_GREATER] = (GenerateConstFunction)generateConstGreater,
    [AST_LESS] = (GenerateConstFunction)generateConstLess,
    [AST_OR] = (GenerateConstFunction)generateConstOr,
    [AST_AND] = (GenerateConstFunction)generateConstAnd,
    [AST_XOR] = (GenerateConstFunction)generateConstXor,
    [AST_SHIFT_RIGHT] = (GenerateConstFunction)generateConstShiftRight,
    [AST_SHIFT_LEFT] = (GenerateConstFunction)generateConstShiftLeft,
    [AST_ADD] = (GenerateConstFunction)generateConstAdd,
    [AST_SUBTRACT] = (GenerateConstFunction)generateConstSubtract,
    [AST_MULTIPLY] = (GenerateConstFunction)generateConstMultiply,
    [AST_DIVIDE] = (GenerateConstFunction)generateConstDivide,
    [AST_REMAINDER] = (GenerateConstFunction)generateConstRemainder,
    [AST_ARRAY] = (GenerateConstFunction)NULL,

    // Unary operation
    [AST_RETURN] = (GenerateConstFunction)NULL,
    [AST_DEREFERENCE] = (GenerateConstFunction)generateConstDereference,
    [AST_POSITIVE] = (GenerateConstFunction)generateConstPositive,
    [AST_NEGATIVE] = (GenerateConstFunction)generateConstNegative,
    [AST_REFERENCE] = (GenerateConstFunction)generateConstReference,
    [AST_NOT] = (GenerateConstFunction)generateConstNot,
    [AST_INCREMENT] = (GenerateConstFunction)NULL,
    [AST_DECREMENT] = (GenerateConstFunction)NULL,

    // Simple Ast
    [AST_ADDRESS] = (GenerateConstFunction)NULL,
};

LLVMValueRef generateConst(Ast* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    if(ast == NULL) {
        return NULL;
    }
    GenerateConstFunction generation_function = generation_functions[ast->type];
    if(generation_function == NULL) {
        addError(error_context, "Unexpected expression", ast->start, ERROR);
        return NULL;
    } else {
        return generation_function(ast, args, symbols, error_context);
    }
}
