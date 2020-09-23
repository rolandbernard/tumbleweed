
#include <llvm-c/DebugInfo.h>

#include "codegen/assignments.h"
#include "codegen/general.h"
#include "codegen/casts.h"

LLVMValueRef generateValueAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        return LLVMBuildStore(builder, value, dest);
    }
    return NULL;
}

LLVMValueRef generateValueOrAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind) {
        addError(error_context, "The or operation only works for integers", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result = LLVMBuildOr(builder, original, value, "");
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueAndAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind) {
        addError(error_context, "The and operation only works for integers", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result = LLVMBuildAnd(builder, original, value, "");
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueXorAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind) {
        addError(error_context, "The xor operation only works for integers", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result = LLVMBuildXor(builder, original, value, "");
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueShiftRightAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind) {
        addError(error_context, "The shift right operation only works for integers", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result = LLVMBuildLShr(builder, original, value, "");
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueShiftLeftAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind) {
        addError(error_context, "The shift left operation only works for integers", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result = LLVMBuildShl(builder, original, value, "");
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueAddAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind &&
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMHalfTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFloatTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMDoubleTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMX86_FP80TypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFP128TypeKind) {
        addError(error_context, "The add operation only works for integers and floats", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result;
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) == LLVMIntegerTypeKind) {
            result = LLVMBuildAdd(builder, original, value, "");
        } else {
            result = LLVMBuildFAdd(builder, original, value, "");
        }
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueSubtractAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind &&
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMHalfTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFloatTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMDoubleTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMX86_FP80TypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFP128TypeKind) {
        addError(error_context, "The subtract operation only works for integers and floats", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result;
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) == LLVMIntegerTypeKind) {
            result = LLVMBuildSub(builder, original, value, "");
        } else {
            result = LLVMBuildFSub(builder, original, value, "");
        }
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueMultiplyAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind &&
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMHalfTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFloatTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMDoubleTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMX86_FP80TypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFP128TypeKind) {
        addError(error_context, "The multiply operation only works for integers and floats", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result;
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) == LLVMIntegerTypeKind) {
            result = LLVMBuildMul(builder, original, value, "");
        } else {
            result = LLVMBuildFMul(builder, original, value, "");
        }
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueDivideAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind &&
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMHalfTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFloatTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMDoubleTypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMX86_FP80TypeKind && 
        LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMFP128TypeKind) {
        addError(error_context, "The divide operation only works for integers and floats", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result;
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) == LLVMIntegerTypeKind) {
            result = LLVMBuildSDiv(builder, original, value, "");
        } else {
            result = LLVMBuildFDiv(builder, original, value, "");
        }
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}

LLVMValueRef generateValueRemainderAssignment(AstAssignment* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef dest = generateValueInFunction(ast->destination, function, dibuilder, builder, args, symbols, error_context);
    if(dest == NULL) {
        error = true;
    } else if(!LLVMIsAAllocaInst(dest) && !LLVMIsAGetElementPtrInst(dest) && !LLVMIsAGlobalVariable(dest) && !(LLVMIsAConstantExpr(dest) && LLVMGetConstOpcode(dest) == LLVMGetElementPtr)) {
        addError(error_context, "Can't write to a temporary value", ast->destination->start, ERROR);
        error = true;
    } else if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(dest))) != LLVMIntegerTypeKind) {
        addError(error_context, "The remainder operation only works for integers", ast->destination->start, ERROR);
        error = true;
    }
    LLVMValueRef value = generateValueInFunction(ast->value, function, dibuilder, builder, args, symbols, error_context);
    if(value == NULL) {
        error = true;
    } else {
        value = generateExtractFromVariable(value, function, builder, args, symbols, error_context);
        value = generateCastFromTo(ast->value, value, LLVMGetElementType(LLVMTypeOf(dest)), function, builder, args, symbols, error_context);
        if (value == NULL) {
            error = true;
        }
    }
    if(!error) {
        LLVMValueRef original = LLVMBuildLoad2(builder, LLVMTypeOf(value), dest, "");
        LLVMValueRef result = LLVMBuildSRem(builder, original, value, "");
        return LLVMBuildStore(builder, result, dest);
    }
    return NULL;
}
