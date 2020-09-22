
#include <string.h>

#include "codegen/const.h"
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

static bool generateOperandsIntOrFloatOrPointer(Ast* lhs, Ast* rhs, Args* args, SymbolTable* symbols, ErrorContext* error_context, LLVMValueRef* left, LLVMValueRef* right) {
    bool error = false;
    *left = generateConst(lhs, args, symbols, error_context);
    if(*left == NULL) {
        error = true;
    } else {
        if (LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMPointerTypeKind &&
            !isAFloat(LLVMTypeOf(*left))) {            
            addError(error_context, "The operation only works for integers, pointers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    *right = generateConst(rhs, args, symbols, error_context);
    if(*right == NULL) {
        error = true;
    } else {
        if (LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMPointerTypeKind &&
            !isAFloat(LLVMTypeOf(*right))) {            
            addError(error_context, "The operation only works for integers, pointers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(*left) != LLVMTypeOf(*right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(*right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind) {
                *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(*left)) > LLVMGetIntTypeWidth(LLVMTypeOf(*right))) {
                    *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
                } else {
                    *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
                }
            } else if(isAFloat(LLVMTypeOf(*left)) && isAFloat(LLVMTypeOf(*right))) {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*right))]) {
                    *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
                } else {
                    *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
                }
            } else {
                *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
            }
        }
    }
    return error;
}

static bool generateOperandsIntOrFloat(Ast* lhs, Ast* rhs, Args* args, SymbolTable* symbols, ErrorContext* error_context, LLVMValueRef* left, LLVMValueRef* right) {
    bool error = false;
    *left = generateConst(lhs, args, symbols, error_context);
    if(*left == NULL) {
        error = true;
    } else {
        if (LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMIntegerTypeKind && !isAFloat((LLVMTypeOf(*left)))) {            
            addError(error_context, "The operation only works for integers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    *right = generateConst(rhs, args, symbols, error_context);
    if(*right == NULL) {
        error = true;
    } else {
        if (LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMIntegerTypeKind && !isAFloat((LLVMTypeOf(*right)))) {            
            addError(error_context, "The operation only works for integers and floats", lhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(*left) != LLVMTypeOf(*right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(*right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind) {
                *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(*left)) > LLVMGetIntTypeWidth(LLVMTypeOf(*right))) {
                    *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
                } else {
                    *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
                }
            } else if(isAFloat(LLVMTypeOf(*left)) && isAFloat(LLVMTypeOf(*right))) {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*right))]) {
                    *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
                } else {
                    *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
                }
            } else {
                *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
            }
        }
    }
    return error;
} 

static bool generateOperandsInt(Ast* lhs, Ast* rhs, Args* args, SymbolTable* symbols, ErrorContext* error_context, LLVMValueRef* left, LLVMValueRef* right) {
    bool error = false;
    *left = generateConst(lhs, args, symbols, error_context);
    if(*left == NULL) {
        error = true;
    } else {
        if (LLVMGetTypeKind((LLVMTypeOf(*left))) != LLVMIntegerTypeKind) {            
            addError(error_context, "The operation only works for integers", lhs->start, ERROR);
            error = true;
        }
    }
    *right = generateConst(rhs, args, symbols, error_context);
    if(*right == NULL) {
        error = true;
    } else {
        if (LLVMGetTypeKind((LLVMTypeOf(*right))) != LLVMIntegerTypeKind) {            
            addError(error_context, "The operation only works for integers", lhs->start, ERROR);
            error = true;
        }
    }
    if(!error) {
        if(LLVMTypeOf(*left) != LLVMTypeOf(*right)) {
            if (LLVMGetTypeKind(LLVMTypeOf(*right)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind) {
                *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) != LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
            } else if (LLVMGetTypeKind(LLVMTypeOf(*left)) == LLVMIntegerTypeKind && LLVMGetTypeKind(LLVMTypeOf(*right)) == LLVMIntegerTypeKind) {
                if (LLVMGetIntTypeWidth(LLVMTypeOf(*left)) > LLVMGetIntTypeWidth(LLVMTypeOf(*right))) {
                    *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
                } else {
                    *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
                }
            } else if(isAFloat(LLVMTypeOf(*left)) && isAFloat(LLVMTypeOf(*right))) {
                if (llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*left))] > llvm_float_type_to_length[LLVMGetTypeKind(LLVMTypeOf(*right))]) {
                    *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
                } else {
                    *left = generateConstCastFromTo(lhs, *left, LLVMTypeOf(*right), args, symbols, error_context);
                }
            } else {
                *right = generateConstCastFromTo(rhs, *right, LLVMTypeOf(*left), args, symbols, error_context);
            }
        }
    }
    return error;
} 

LLVMValueRef generateConstLazyAnd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef condition_lhs = generateConst(ast->lhs, args, symbols, error_context);
    LLVMValueRef condition_lhs_bool = NULL;
    if (condition_lhs == NULL) {
        error = true;
    } else {
        LLVMTypeRef condition_type = LLVMTypeOf(condition_lhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_lhs_bool = condition_lhs;
            } else {
                condition_lhs_bool = LLVMConstICmp(LLVMIntNE, condition_lhs, LLVMConstNull(condition_type));
            }
            break;
        case LLVMPointerTypeKind:
            condition_lhs_bool = LLVMConstICmp(LLVMIntNE, condition_lhs, LLVMConstNull(condition_type));
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_lhs_bool = LLVMConstFCmp(LLVMRealUNE, condition_lhs, LLVMConstNull(condition_type));
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->lhs->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMValueRef condition_rhs = generateConst(ast->rhs, args, symbols, error_context);
    LLVMValueRef condition_rhs_bool = NULL;
    if (condition_rhs == NULL) {
        error = true;
    } else {
        LLVMTypeRef condition_type = LLVMTypeOf(condition_rhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_rhs_bool = condition_rhs;
            } else {
                condition_rhs_bool = LLVMConstICmp(LLVMIntNE, condition_rhs, LLVMConstNull(condition_type));
            }
            break;
        case LLVMPointerTypeKind:
            condition_rhs_bool = LLVMConstICmp(LLVMIntNE, condition_rhs, LLVMConstNull(condition_type));
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_rhs_bool = LLVMConstFCmp(LLVMRealUNE, condition_rhs, LLVMConstNull(condition_type));
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->lhs->start, ERROR);
            error = true;
            break;
        }
    }
    if(!error) {
        return LLVMConstSelect(condition_lhs_bool, condition_rhs_bool, LLVMConstNull(LLVMIntType(1)));
    }
    return NULL;
}

LLVMValueRef generateConstLazyOr(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef condition_lhs = generateConst(ast->lhs, args, symbols, error_context);
    LLVMValueRef condition_lhs_bool = NULL;
    if (condition_lhs == NULL) {
        error = true;
    } else {
        LLVMTypeRef condition_type = LLVMTypeOf(condition_lhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_lhs_bool = condition_lhs;
            } else {
                condition_lhs_bool = LLVMConstICmp(LLVMIntNE, condition_lhs, LLVMConstNull(condition_type));
            }
            break;
        case LLVMPointerTypeKind:
            condition_lhs_bool = LLVMConstICmp(LLVMIntNE, condition_lhs, LLVMConstNull(condition_type));
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_lhs_bool = LLVMConstFCmp(LLVMRealUNE, condition_lhs, LLVMConstNull(condition_type));
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->lhs->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMValueRef condition_rhs = generateConst(ast->rhs, args, symbols, error_context);
    LLVMValueRef condition_rhs_bool = NULL;
    if (condition_rhs == NULL) {
        error = true;
    } else {
        LLVMTypeRef condition_type = LLVMTypeOf(condition_rhs);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
            if(LLVMGetIntTypeWidth(condition_type) == 1) {
                condition_rhs_bool = condition_rhs;
            } else {
                condition_rhs_bool = LLVMConstICmp(LLVMIntNE, condition_rhs, LLVMConstNull(condition_type));
            }
            break;
        case LLVMPointerTypeKind:
            condition_rhs_bool = LLVMConstICmp(LLVMIntNE, condition_rhs, LLVMConstNull(condition_type));
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_rhs_bool = LLVMConstFCmp(LLVMRealUNE, condition_rhs, LLVMConstNull(condition_type));
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->lhs->start, ERROR);
            error = true;
            break;
        }
    }
    if(!error) {
        return LLVMConstSelect(condition_lhs_bool, LLVMConstInt(LLVMIntType(1), 1, 0), condition_rhs_bool);
    }
    return NULL;
}

LLVMValueRef generateConstEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMConstFCmp(LLVMRealOEQ, left, right);
        } else {
            return LLVMConstICmp(LLVMIntEQ, left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstUnequal(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMConstFCmp(LLVMRealONE, left, right);
        } else {
            return LLVMConstICmp(LLVMIntNE, left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstGreaterEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMConstFCmp(LLVMRealOGE, left, right);
        } else {
            return LLVMConstICmp(LLVMIntSGE, left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstLessEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMConstFCmp(LLVMRealOLE, left, right);
        } else {
            return LLVMConstICmp(LLVMIntSLE, left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstGreater(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMConstFCmp(LLVMRealOGT, left, right);
        } else {
            return LLVMConstICmp(LLVMIntSGT, left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstLess(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloatOrPointer(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (isAFloat(LLVMTypeOf(right))) {
            return LLVMConstFCmp(LLVMRealOLT, left, right);
        } else {
            return LLVMConstICmp(LLVMIntSLT, left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstOr(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMConstOr(left, right);
    }
    return NULL;
}

LLVMValueRef generateConstAnd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMConstAnd(left, right);
    }
    return NULL;
}

LLVMValueRef generateConstXor(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMConstXor(left, right);
    }
    return NULL;
}

LLVMValueRef generateConstShiftRight(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMConstLShr(left, right);
    }
    return NULL;
}

LLVMValueRef generateConstShiftLeft(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMConstShl(left, right);
    }
    return NULL;
}

LLVMValueRef generateConstAdd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMConstAdd(left, right);
        } else {
            return LLVMConstFAdd(left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstSubtract(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMConstSub(left, right);
        } else {
            return LLVMConstFSub(left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstMultiply(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMConstMul(left, right);
        } else {
            return LLVMConstFMul(left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstDivide(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsIntOrFloat(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        if (LLVMGetTypeKind((LLVMTypeOf(right))) == LLVMIntegerTypeKind) {
            return LLVMConstSDiv(left, right);
        } else {
            return LLVMConstFDiv(left, right);
        }
    }
    return NULL;
}

LLVMValueRef generateConstRemainder(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef left;
    LLVMValueRef right;
    bool error = generateOperandsInt(ast->lhs, ast->rhs, args, symbols, error_context, &left, &right);
    if(!error) {
        return LLVMConstSRem(left, right);
    }
    return NULL;
}

LLVMValueRef generateConstPositive(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateConst(ast->operand, args, symbols, error_context);
    if(value != NULL) {
        return value;
    }
    return NULL;
}

LLVMValueRef generateConstNegative(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateConst(ast->operand, args, symbols, error_context);
    if(value != NULL) {
        if (LLVMGetTypeKind((LLVMTypeOf(value))) != LLVMIntegerTypeKind && !isAFloat(LLVMTypeOf(value))) {            
            addError(error_context, "The operation only works for integers and floats", ast->operand->start, ERROR);
        } else {
            if (isAFloat(LLVMTypeOf(value))) {
                return LLVMConstFNeg(value);
            } else {
                return LLVMConstNeg(value);
            }
        }
    }
    return NULL;
}

LLVMValueRef generateConstNot(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMValueRef value = generateConst(ast->operand, args, symbols, error_context);
    if(value != NULL) {
        if (LLVMGetTypeKind((LLVMTypeOf(value))) != LLVMIntegerTypeKind &&
            LLVMGetTypeKind((LLVMTypeOf(value))) != LLVMPointerTypeKind && !isAFloat(LLVMTypeOf(value))) {            
            addError(error_context, "The operation only works for integers, pointers and floats", ast->operand->start, ERROR);
        } else {
            if (isAFloat(LLVMTypeOf(value))) {
                return LLVMConstFCmp(LLVMRealUEQ, value, LLVMConstNull(LLVMTypeOf(value)));
            } else {
                return LLVMConstNot(value);
            }
        }
    }
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

LLVMValueRef generateConstVariableAccess(AstVariableAccess* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    Symbol* symbol = getSymbol(symbols, ast->name);
    if(symbol == NULL) {
        addErrorf(error_context, ast->start, ERROR, "Undefined symbol '%s'", ast->name);
        return NULL;
    } else if (symbol->llvm_value != NULL && LLVMIsAFunction(symbol->llvm_value)) {
        return symbol->llvm_value;
    } else {
        return NULL;
    }
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
    [AST_VARIABLE_ACCESS] = (GenerateConstFunction)generateConstVariableAccess,
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
    [AST_DEREFERENCE] = (GenerateConstFunction)NULL,
    [AST_POSITIVE] = (GenerateConstFunction)generateConstPositive,
    [AST_NEGATIVE] = (GenerateConstFunction)generateConstNegative,
    [AST_REFERENCE] = (GenerateConstFunction)NULL,
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
