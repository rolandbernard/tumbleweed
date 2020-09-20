
#include "codegen/operations.h"
#include "codegen/general.h"
#include "codegen/casts.h"

int llvm_float_type_to_length[] = {
    [LLVMHalfTypeKind] = 16,
    [LLVMFloatTypeKind] = 32,
    [LLVMDoubleTypeKind] = 64,
    [LLVMX86_FP80TypeKind] = 80,
    [LLVMFP128TypeKind] = 128,
};

LLVMValueRef generateValueLazyAnd(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueLazyOr(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueEqual(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueUnequal(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueGreaterEqual(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueLessEqual(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueGreater(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueLess(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueOr(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueAnd(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueXor(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueShiftRight(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueShiftLeft(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueAdd(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef left = generateValueInFunction(ast->lhs, function, builder, args, symbols, error_context);
    if(left == NULL) {
        error = true;
    } else {
        left = generateExtractFromVariable(left, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(left))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(left))) != LLVMHalfTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(left))) != LLVMFloatTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(left))) != LLVMDoubleTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(left))) != LLVMX86_FP80TypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(left))) != LLVMFP128TypeKind) {            
            addError(error_context, "The add operation only works for integers and floats", ast->lhs->start, ERROR);
            error = true;
        }
    }
    LLVMValueRef right = generateValueInFunction(ast->rhs, function, builder, args, symbols, error_context);
    if(right == NULL) {
        error = true;
    } else {
        right = generateExtractFromVariable(right, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(right))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(right))) != LLVMHalfTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(right))) != LLVMFloatTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(right))) != LLVMDoubleTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(right))) != LLVMX86_FP80TypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(right))) != LLVMFP128TypeKind) {            
            addError(error_context, "The add operation only works for integers and floats", ast->rhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(left) != LLVMTypeOf(right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(left)) == LLVMIntegerTypeKind) {
                left = generateCastFromTo(ast->lhs, left, LLVMTypeOf(right), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(right)) == LLVMIntegerTypeKind) {
                right = generateCastFromTo(ast->rhs, right, LLVMTypeOf(left), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(left)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(left)) > LLVMGetIntTypeWidth(LLVMTypeOf(right))) {
                    right = generateCastFromTo(ast->rhs, right, LLVMTypeOf(left), function, builder, args, symbols, error_context);
                } else {
                    left = generateCastFromTo(ast->lhs, left, LLVMTypeOf(right), function, builder, args, symbols, error_context);
                }
            } else {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(right))]) {
                    right = generateCastFromTo(ast->rhs, right, LLVMTypeOf(left), function, builder, args, symbols, error_context);
                } else {
                    left = generateCastFromTo(ast->lhs, left, LLVMTypeOf(right), function, builder, args, symbols, error_context);
                }
            }
        }
        if(!error) {
            if(LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
                return LLVMBuildAdd(builder, left, right, "");
            } else {
                return LLVMBuildFAdd(builder, left, right, "");
            }
        }
    }
    return NULL;
}

LLVMValueRef generateValueSubtract(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueMultiply(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueDivide(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueRemainder(AstBinaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueReference(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueDereference(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValuePositive(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueNegative(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueNot(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueIncrement(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueDecrement(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL; 
}
