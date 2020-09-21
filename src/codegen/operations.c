
#include <llvm-c/DebugInfo.h>

#include "codegen/operations.h"
#include "codegen/general.h"
#include "codegen/casts.h"

static int llvm_float_type_to_length[] = {
    [LLVMHalfTypeKind] = 16,
    [LLVMFloatTypeKind] = 32,
    [LLVMDoubleTypeKind] = 64,
    [LLVMX86_FP80TypeKind] = 80,
    [LLVMFP128TypeKind] = 128,
};

static bool isAFloat(LLVMTypeRef type) {
    return LLVMGetTypeKind(type) == LLVMFloatTypeKind || LLVMGetTypeKind(type) == LLVMDoubleTypeKind || 
            LLVMGetTypeKind(type) == LLVMHalfTypeKind || LLVMGetTypeKind(type) == LLVMX86_FP80TypeKind ||
            LLVMGetTypeKind(type) == LLVMFP128TypeKind || LLVMGetTypeKind(type) == LLVMPPC_FP128TypeKind;
}

static bool generateOperandsIntOrFloatOrPointer(Ast* lhs, Ast* rhs, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context, LLVMValueRef* left, LLVMValueRef* right) {
    bool error = false;
    *left = generateValueInFunction(lhs, function, dibuilder, builder, args, symbols, error_context);
    if(*left == NULL) {
        error = true;
    } else {
        *left = generateExtractFromVariable(*left, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMPointerTypeKind &&
            !isAFloat(LLVMTypeOf(*right))) {            
            addError(error_context, "The operation only works for integers, pointers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    *right = generateValueInFunction(rhs, function, dibuilder, builder, args, symbols, error_context);
    if(*right == NULL) {
        error = true;
    } else {
        *right = generateExtractFromVariable(*right, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMPointerTypeKind &&
            !isAFloat(LLVMTypeOf(*right))) {            
            addError(error_context, "The operation only works for integers, pointers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(*left) != LLVMTypeOf(*right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(*right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind) {
                *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(*left)) > LLVMGetIntTypeWidth(LLVMTypeOf(*right))) {
                    *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
                } else {
                    *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
                }
            } else if(isAFloat(LLVMTypeOf(*left)) && isAFloat(LLVMTypeOf(*right))) {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*right))]) {
                    *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
                } else {
                    *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
                }
            } else {
                *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
            }
        }
    }
    return error;
}

static bool generateOperandsIntOrFloat(Ast* lhs, Ast* rhs, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context, LLVMValueRef* left, LLVMValueRef* right) {
    bool error = false;
    *left = generateValueInFunction(lhs, function, dibuilder, builder, args, symbols, error_context);
    if(*left == NULL) {
        error = true;
    } else {
        *left = generateExtractFromVariable(*left, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMHalfTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMFloatTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMDoubleTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMX86_FP80TypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMFP128TypeKind) {            
            addError(error_context, "The operation only works for integers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    *right = generateValueInFunction(rhs, function, dibuilder, builder, args, symbols, error_context);
    if(*right == NULL) {
        error = true;
    } else {
        *right = generateExtractFromVariable(*right, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMHalfTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMFloatTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMDoubleTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMX86_FP80TypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMFP128TypeKind) {            
            addError(error_context, "The operation only works for integers and floats", rhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(*left) != LLVMTypeOf(*right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(*right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind) {
                *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(*left)) > LLVMGetIntTypeWidth(LLVMTypeOf(*right))) {
                    *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
                } else {
                    *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
                }
            } else if(isAFloat(LLVMTypeOf(*left)) && isAFloat(LLVMTypeOf(*right))) {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*right))]) {
                    *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
                } else {
                    *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
                }
            } else {
                *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
            }
        }
    }
    return error;
}

static bool generateOperandsInt(Ast* lhs, Ast* rhs, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context, LLVMValueRef* left, LLVMValueRef* right) {
    bool error = false;
    *left = generateValueInFunction(lhs, function, dibuilder, builder, args, symbols, error_context);
    if(*left == NULL) {
        error = true;
    } else {
        *left = generateExtractFromVariable(*left, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMIntegerTypeKind) {            
            addError(error_context, "The operation only works for integers", lhs->start, ERROR);
            error = true;
        }
    }
    *right = generateValueInFunction(rhs, function, dibuilder, builder, args, symbols, error_context);
    if(*right == NULL) {
        error = true;
    } else {
        *right = generateExtractFromVariable(*right, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMIntegerTypeKind) {            
            addError(error_context, "The operation only works for integers", rhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(*left) != LLVMTypeOf(*right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(*right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind) {
                *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(*left)) > LLVMGetIntTypeWidth(LLVMTypeOf(*right))) {
                    *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
                } else {
                    *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
                }
            } else if(isAFloat(LLVMTypeOf(*left)) && isAFloat(LLVMTypeOf(*right))) {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*right))]) {
                    *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
                } else {
                    *left = generateCastFromTo(lhs, *left, LLVMTypeOf(*right), function, builder, args, symbols, error_context);
                }
            } else {
                *right = generateCastFromTo(rhs, *right, LLVMTypeOf(*left), function, builder, args, symbols, error_context);
            }
        }
    }
    return error;
}

LLVMValueRef generateValueLazyAnd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef condition_lhs = generateValueInFunction(ast->lhs, function, dibuilder, builder, args, symbols, error_context);
    LLVMValueRef condition_lhs_bool = NULL;
    if (condition_lhs == NULL) {
        error = true;
    } else {
        condition_lhs = generateExtractFromVariable(condition_lhs, function, builder, args, symbols, error_context);
        LLVMTypeRef condition_type = LLVMTypeOf(condition_lhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_lhs_bool = condition_lhs;
            } else {
                condition_lhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_lhs, LLVMConstNull(condition_type), "");
            }
            break;
        case LLVMPointerTypeKind:
            condition_lhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_lhs, LLVMConstNull(condition_type), "");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_lhs_bool = LLVMBuildFCmp(builder, LLVMRealUNE, condition_lhs, LLVMConstNull(condition_type), "");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->lhs->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMBasicBlockRef start = LLVMGetInsertBlock(builder);
    LLVMBasicBlockRef alt = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(start), "");
    LLVMBasicBlockRef end = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(alt), "");
    if (condition_lhs_bool == NULL) {
        error = true;
    } else {
        LLVMBuildCondBr(builder, condition_lhs_bool, alt, end);
    }
    LLVMPositionBuilderAtEnd(builder, alt);
    LLVMValueRef condition_rhs = generateValueInFunction(ast->rhs, function, dibuilder, builder, args, symbols, error_context);
    LLVMValueRef condition_rhs_bool = NULL;
    if (condition_rhs == NULL) {
        error = true;
    } else {
        condition_rhs = generateExtractFromVariable(condition_rhs, function, builder, args, symbols, error_context);
        LLVMTypeRef condition_type = LLVMTypeOf(condition_rhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_rhs_bool = condition_rhs;
            } else {
                condition_rhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_rhs, LLVMConstNull(condition_type), "");
            }
            break;
        case LLVMPointerTypeKind:
            condition_rhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_rhs, LLVMConstNull(condition_type), "");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_rhs_bool = LLVMBuildFCmp(builder, LLVMRealUNE, condition_rhs, LLVMConstNull(condition_type), "");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->rhs->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMBuildBr(builder, end);
    LLVMPositionBuilderAtEnd(builder, end);
    if (condition_rhs_bool == NULL) {
        error = true;
    }
    if(!error) {
        LLVMValueRef phi = LLVMBuildPhi(builder, LLVMIntType(1), "");
        LLVMValueRef values[2] = {condition_rhs_bool, LLVMConstInt(LLVMIntType(1), 0, 0)};
        LLVMBasicBlockRef blocks[2] = {alt, start};
        LLVMAddIncoming(phi, values, blocks, 2);
        return phi;
    }
    return NULL;
}

LLVMValueRef generateValueLazyOr(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef condition_lhs = generateValueInFunction(ast->lhs, function, dibuilder, builder, args, symbols, error_context);
    LLVMValueRef condition_lhs_bool = NULL;
    if (condition_lhs == NULL) {
        error = true;
    } else {
        condition_lhs = generateExtractFromVariable(condition_lhs, function, builder, args, symbols, error_context);
        LLVMTypeRef condition_type = LLVMTypeOf(condition_lhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_lhs_bool = condition_lhs;
            } else {
                condition_lhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_lhs, LLVMConstNull(condition_type), "");
            }
            break;
        case LLVMPointerTypeKind:
            condition_lhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_lhs, LLVMConstNull(condition_type), "");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_lhs_bool = LLVMBuildFCmp(builder, LLVMRealUNE, condition_lhs, LLVMConstNull(condition_type), "");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->lhs->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMBasicBlockRef start = LLVMGetInsertBlock(builder);
    LLVMBasicBlockRef alt = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(start), "");
    LLVMBasicBlockRef end = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(alt), "");
    if (condition_lhs_bool == NULL) {
        error = true;
    } else {
        LLVMBuildCondBr(builder, condition_lhs_bool, end, alt);
    }
    LLVMPositionBuilderAtEnd(builder, alt);
    LLVMValueRef condition_rhs = generateValueInFunction(ast->rhs, function, dibuilder, builder, args, symbols, error_context);
    LLVMValueRef condition_rhs_bool = NULL;
    if (condition_rhs == NULL) {
        error = true;
    } else {
        condition_rhs = generateExtractFromVariable(condition_rhs, function, builder, args, symbols, error_context);
        LLVMTypeRef condition_type = LLVMTypeOf(condition_rhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_rhs_bool = condition_rhs;
            } else {
                condition_rhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_rhs, LLVMConstNull(condition_type), "");
            }
            break;
        case LLVMPointerTypeKind:
            condition_rhs_bool = LLVMBuildICmp(builder, LLVMIntNE, condition_rhs, LLVMConstNull(condition_type), "");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_rhs_bool = LLVMBuildFCmp(builder, LLVMRealUNE, condition_rhs, LLVMConstNull(condition_type), "");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->rhs->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMBuildBr(builder, end);
    LLVMPositionBuilderAtEnd(builder, end);
    if (condition_rhs_bool == NULL) {
        error = true;
    }
    if(!error) {
        LLVMValueRef phi = LLVMBuildPhi(builder, LLVMIntType(1), "");
        LLVMValueRef values[2] = {condition_rhs_bool, LLVMConstInt(LLVMIntType(1), 1, 0)};
        LLVMBasicBlockRef blocks[2] = {alt, start};
        LLVMAddIncoming(phi, values, blocks, 2);
        return phi;
    }
    return NULL;
}

LLVMValueRef generateValueEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMBuildFCmp(builder, LLVMRealOEQ, left, right, "");
        } else {
            return LLVMBuildICmp(builder, LLVMIntEQ, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueUnequal(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMBuildFCmp(builder, LLVMRealONE, left, right, "");
        } else {
            return LLVMBuildICmp(builder, LLVMIntNE, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueGreaterEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMBuildFCmp(builder, LLVMRealOGE, left, right, "");
        } else {
            return LLVMBuildICmp(builder, LLVMIntSGE, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueLessEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMBuildFCmp(builder, LLVMRealOLE, left, right, "");
        } else {
            return LLVMBuildICmp(builder, LLVMIntSLE, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueGreater(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMBuildFCmp(builder, LLVMRealOGT, left, right, "");
        } else {
            return LLVMBuildICmp(builder, LLVMIntSGT, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueLess(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMBuildFCmp(builder, LLVMRealOLT, left, right, "");
        } else {
            return LLVMBuildICmp(builder, LLVMIntSLT, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueOr(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMBuildOr(builder, left, right, "");
    }
    return NULL;
}

LLVMValueRef generateValueAnd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMBuildAnd(builder, left, right, "");
    }
    return NULL;
}

LLVMValueRef generateValueXor(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMBuildXor(builder, left, right, "");
    }
    return NULL;
}

LLVMValueRef generateValueShiftRight(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMBuildLShr(builder, left, right, "");
    }
    return NULL;
}

LLVMValueRef generateValueShiftLeft(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMBuildShl(builder, left, right, "");
    }
    return NULL;
}

LLVMValueRef generateValueAdd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMBuildAdd(builder, left, right, "");
        } else {
            return LLVMBuildFAdd(builder, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueSubtract(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMBuildSub(builder, left, right, "");
        } else {
            return LLVMBuildFSub(builder, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueMultiply(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMBuildMul(builder, left, right, "");
        } else {
            return LLVMBuildFMul(builder, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueDivide(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMBuildSDiv(builder, left, right, "");
        } else {
            return LLVMBuildFDiv(builder, left, right, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValueRemainder(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, function, dibuilder, builder, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMBuildSRem(builder, left, right, "");
    }
    return NULL;
}

LLVMValueRef generateValueReference(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value != NULL) {
        if(LLVMIsAAllocaInst(value) || LLVMIsAGetElementPtrInst(value) || LLVMIsAGlobalVariable(value)) {
            LLVMValueRef value_int = LLVMBuildPtrToInt(builder, value, LLVMIntType(64), "");
            return LLVMBuildIntToPtr(builder, value_int, LLVMTypeOf(value), "");
        } else {
            addError(error_context, "Temporary values can't be written to", ast->start, ERROR);
        }
    }
    return NULL;
}

LLVMValueRef generateValueDereference(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind(LLVMTypeOf(value)) != LLVMPointerTypeKind || LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(value))) == LLVMFunctionTypeKind) {
            addError(error_context, "Only pointers or arrays can be dereferenced", ast->operand->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(value))) == LLVMArrayTypeKind) {
            LLVMValueRef indecies[2] = {LLVMConstNull(LLVMIntType(1)), LLVMConstNull(LLVMIntType(1))};
            return LLVMBuildGEP2(builder, LLVMGetElementType(LLVMTypeOf(value)), value, indecies, 2, "");
        } else {
            LLVMValueRef index = LLVMConstNull(LLVMIntType(1));
            return LLVMBuildGEP2(builder, LLVMGetElementType(LLVMTypeOf(value)), value, &index, 1, "");
        }
    }
    return NULL;
}

LLVMValueRef generateValuePositive(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value != NULL) {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        return value;
    }
    return NULL;
}

LLVMValueRef generateValueNegative(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value != NULL) {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(value))) != LLVMIntegerTypeKind && !isAFloat(LLVMTypeOf(value))) {            
            addError(error_context, "The operation only works for integers and floats", ast->operand->start, ERROR);
        } else {
            if (isAFloat(LLVMTypeOf(value))) {
                return LLVMBuildFNeg(builder, value, "");
            } else {
                return LLVMBuildNeg(builder, value, "");
            }
        }
    }
    return NULL;
}

LLVMValueRef generateValueNot(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value != NULL) {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        if (LLVMGetTypeKind((LLVMTypeOf(value))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(value))) != LLVMPointerTypeKind && !isAFloat(LLVMTypeOf(value))) {            
            addError(error_context, "The operation only works for integers, pointers and floats", ast->operand->start, ERROR);
        } else {
            if (isAFloat(LLVMTypeOf(value))) {
                return LLVMBuildFCmp(builder, LLVMRealUEQ, value, LLVMConstNull(LLVMTypeOf(value)), "");
            } else {
                return LLVMBuildNot(builder, value, "");
            }
        }
    }
    return NULL;
}

LLVMValueRef generateValueIncrement(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value != NULL) {
        if(!LLVMIsAAllocaInst(value) && !LLVMIsAGetElementPtrInst(value) && !LLVMIsAGlobalVariable(value)) {
            addError(error_context, "Can't write to a temporary value", ast->operand->start, ERROR);
        } else {
            LLVMValueRef value_ext = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
            if (LLVMGetTypeKind((LLVMTypeOf(value_ext))) != LLVMIntegerTypeKind &&
                LLVMGetTypeKind((LLVMTypeOf(value_ext))) != LLVMPointerTypeKind && !isAFloat(LLVMTypeOf(value_ext))) {
                addError(error_context, "Only integers, pointers and floats can be incremented", ast->operand->start, ERROR);
            } else {
                if (isAFloat(LLVMTypeOf(value_ext))) {
                    LLVMValueRef incremented = LLVMBuildFAdd(builder, value_ext, LLVMConstReal(LLVMTypeOf(value_ext), 1.0), "");
                    return LLVMBuildStore(builder, incremented, value);
                } else if (LLVMGetTypeKind((LLVMTypeOf(value_ext))) == LLVMPointerTypeKind) {
                    LLVMValueRef incremented;
                    if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(value_ext))) == LLVMArrayTypeKind) {
                        LLVMValueRef indecies[2] = {LLVMConstNull(LLVMIntType(1)), LLVMConstInt(LLVMIntType(1), 1, 0)};
                        incremented = LLVMBuildGEP2(builder, LLVMGetElementType(LLVMTypeOf(value_ext)), value_ext, indecies, 2, "");
                        incremented = LLVMBuildPointerCast(builder, incremented, LLVMTypeOf(value_ext), "");
                    } else {
                        LLVMValueRef index = LLVMConstInt(LLVMIntType(1), 1, 0);
                        incremented = LLVMBuildGEP2(builder, LLVMGetElementType(LLVMTypeOf(value_ext)), value_ext, &index, 1, "");
                    }
                    return LLVMBuildStore(builder, incremented, value);
                } else {
                    LLVMValueRef incremented = LLVMBuildAdd(builder, value_ext, LLVMConstInt(LLVMTypeOf(value_ext), 1, 0), "");
                    return LLVMBuildStore(builder, incremented, value);
                }
            }
        }
    }
    return NULL;
}

LLVMValueRef generateValueDecrement(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateValueInFunction(ast->operand, function, dibuilder, builder, args, symbols, error_context);
    if(value != NULL) {
        if(!LLVMIsAAllocaInst(value) && !LLVMIsAGetElementPtrInst(value) && !LLVMIsAGlobalVariable(value)) {
            addError(error_context, "Can't write to a temporary value", ast->operand->start, ERROR);
        } else {
            LLVMValueRef value_ext = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
            if (LLVMGetTypeKind((LLVMTypeOf(value_ext))) != LLVMIntegerTypeKind &&
                LLVMGetTypeKind((LLVMTypeOf(value_ext))) != LLVMPointerTypeKind && !isAFloat(LLVMTypeOf(value_ext))) {
                addError(error_context, "Only integers, pointers and floats can be incremented", ast->operand->start, ERROR);
            } else {
                if (isAFloat(LLVMTypeOf(value_ext))) {
                    LLVMValueRef incremented = LLVMBuildFSub(builder, value_ext, LLVMConstReal(LLVMTypeOf(value_ext), 1.0), "");
                    return LLVMBuildStore(builder, incremented, value);
                } else if (LLVMGetTypeKind((LLVMTypeOf(value_ext))) == LLVMPointerTypeKind) {
                    LLVMValueRef incremented;
                    if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(value_ext))) == LLVMArrayTypeKind) {
                        LLVMValueRef indecies[2] = {LLVMConstNull(LLVMIntType(1)), LLVMConstInt(LLVMIntType(64), -1, 1)};
                        incremented = LLVMBuildGEP2(builder, LLVMGetElementType(LLVMTypeOf(value_ext)), value_ext, indecies, 2, "");
                        incremented = LLVMBuildPointerCast(builder, incremented, LLVMTypeOf(value_ext), "");
                    } else {
                        LLVMValueRef index = LLVMConstInt(LLVMIntType(64), -1, 1);
                        incremented = LLVMBuildGEP2(builder, LLVMGetElementType(LLVMTypeOf(value_ext)), value_ext, &index, 1, "");
                    }
                    return LLVMBuildStore(builder, incremented, value);
                } else {
                    LLVMValueRef incremented = LLVMBuildSub(builder, value_ext, LLVMConstInt(LLVMTypeOf(value_ext), 1, 0), "");
                    return LLVMBuildStore(builder, incremented, value);
                }
            }
        }
    }
    return NULL; 
}
