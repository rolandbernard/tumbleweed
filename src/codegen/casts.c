
#include <codegen/casts.h>

LLVMValueRef generateCastFromTo(Ast* ast, LLVMValueRef value, LLVMTypeRef dest, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    LLVMTypeRef src = LLVMTypeOf(value);
    if(src == dest) {
        return value;
    } else if(LLVMGetTypeKind(src) == LLVMGetTypeKind(dest)) {
        if(LLVMGetTypeKind(src) == LLVMPointerTypeKind) {
            return LLVMBuildPointerCast(builder, value, dest, "");
        } else if(LLVMGetTypeKind(src) == LLVMIntegerTypeKind) {
            return LLVMBuildSExt(builder, value, dest, "");
        } else if(LLVMGetTypeKind(src) == LLVMFloatTypeKind || LLVMGetTypeKind(src) == LLVMDoubleTypeKind || 
            LLVMGetTypeKind(src) == LLVMHalfTypeKind || LLVMGetTypeKind(src) == LLVMX86_FP80TypeKind ||
            LLVMGetTypeKind(src) == LLVMFP128TypeKind) {
            return LLVMBuildFPCast(builder, value, dest, "");
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
    }
    addError(error_context, "Unable to cast this value", ast->start, ERROR);
    return NULL;
}

LLVMValueRef generateExtractFromVariable(LLVMValueRef value, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    if(LLVMIsAAllocaInst(value) || LLVMIsAGetElementPtrInst(value)) {
        if(LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(value))) == LLVMArrayTypeKind) {
            return value;
        } else {
            return LLVMBuildLoad2(builder, LLVMGetElementType(LLVMTypeOf(value)), value, "");
        }
    } else {
        return value;
    }
}
