
#include <codegen/casts.h>

static bool isAFloat(LLVMTypeRef type) {
    return LLVMGetTypeKind(type) == LLVMFloatTypeKind || LLVMGetTypeKind(type) == LLVMDoubleTypeKind || 
            LLVMGetTypeKind(type) == LLVMHalfTypeKind || LLVMGetTypeKind(type) == LLVMX86_FP80TypeKind ||
            LLVMGetTypeKind(type) == LLVMFP128TypeKind || LLVMGetTypeKind(type) == LLVMPPC_FP128TypeKind;
}

LLVMValueRef generateCastFromTo(Ast* ast, LLVMValueRef value, LLVMTypeRef dest, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMTypeRef src = LLVMTypeOf(value);
    if(src == dest) {
        return value;
    } else if(LLVMGetTypeKind(src) == LLVMGetTypeKind(dest)) {
        if(LLVMGetTypeKind(src) == LLVMPointerTypeKind) {
            return LLVMBuildPointerCast(builder, value, dest, "");
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind) {
            return LLVMBuildSExt(builder, value, dest, "");
        } else if(isAFloat(src)) {
            return LLVMBuildFPCast(builder, value, dest, "");
        }
    } else {
        if(LLVMGetTypeKind(src) == LLVMPointerTypeKind && LLVMGetTypeKind(dest) == LLVMIntegerTypeKind) {
            return LLVMBuildPtrToInt(builder, value, dest, "");
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind && LLVMGetTypeKind(dest) == LLVMPointerTypeKind) {
            return LLVMBuildIntToPtr(builder, value, dest, "");
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind && isAFloat(dest)) {
            return LLVMBuildSIToFP(builder, value, dest, "");
        } else if(isAFloat(src) && LLVMGetTypeKind(dest) == LLVMIntegerTypeKind) {
            return LLVMBuildFPToSI(builder, value, dest, "");
        }
    }
    addError(error_context, "Unable to cast this value", ast->start, ERROR);
    return NULL;
}

LLVMValueRef generateConstCastFromTo(Ast* ast, LLVMValueRef value, LLVMTypeRef dest, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMTypeRef src = LLVMTypeOf(value);
    if(src == dest) {
        return value;
    } else if(LLVMGetTypeKind(src) == LLVMGetTypeKind(dest)) {
        if(LLVMGetTypeKind(src) == LLVMPointerTypeKind) {
            return LLVMConstPointerCast(value, dest);
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind) {
            return LLVMConstSExt(value, dest);
        } else if(LLVMGetTypeKind(src) == LLVMFloatTypeKind || LLVMGetTypeKind(src) == LLVMDoubleTypeKind || 
            LLVMGetTypeKind(src) == LLVMHalfTypeKind || LLVMGetTypeKind(src) == LLVMX86_FP80TypeKind ||
            LLVMGetTypeKind(src) == LLVMFP128TypeKind) {
            return LLVMConstFPCast(value, dest);
        }
    } else {
        if(LLVMGetTypeKind(src) == LLVMPointerTypeKind && LLVMGetTypeKind(dest) == LLVMIntegerTypeKind) {
            return LLVMConstPtrToInt(value, dest);
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind && LLVMGetTypeKind(dest) == LLVMPointerTypeKind) {
            return LLVMConstIntToPtr(value, dest);
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind && isAFloat(dest)) {
            return LLVMConstSIToFP(value, dest);
        } else if(isAFloat(src) && LLVMGetTypeKind(dest) == LLVMIntegerTypeKind) {
            return LLVMConstFPToSI(value, dest);
        }
    }
    addError(error_context, "Unable to cast this value", ast->start, ERROR);
    return NULL;
}

LLVMValueRef generateExtractFromVariable(LLVMValueRef value, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    if(LLVMIsAAllocaInst(value) || LLVMIsAGetElementPtrInst(value) || LLVMIsAGlobalVariable(value)) {
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(value))) == LLVMArrayTypeKind) {
            return value;
        } else {
            return LLVMBuildLoad2(builder, LLVMGetElementType(LLVMTypeOf(value)), value, "");
        }
    } else {
        return value;
    }
}
