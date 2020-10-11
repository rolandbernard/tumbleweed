
#include <string.h>
#include <stdlib.h>
#include <llvm-c/DebugInfo.h>

#include "codegen/types.h"

LLVMTypeRef generateTypeBase(AstVariableAccess* ast, Args* args, ErrorContext* error_context) {
    if(strcmp(ast->name, "int") == 0) {
        return LLVMIntType(64);
    } else if(strcmp(ast->name, "bool") == 0) {
        return LLVMIntType(1);
    } else if(strcmp(ast->name, "real") == 0 || strcmp(ast->name, "f64") == 0) {
        return LLVMDoubleType();
    } else if(strcmp(ast->name, "f32") == 0) {
        return LLVMFloatType();
    } else if(strcmp(ast->name, "f16") == 0) {
        return LLVMHalfType();
    } else if(strcmp(ast->name, "f80") == 0) {
        return LLVMX86FP80Type();
    } else if(strcmp(ast->name, "f128") == 0) {
        return LLVMFP128Type();
    } else if(ast->name[0] == 'i') {
        bool is_number = true;
        int i = 1;
        while(ast->name[i] != 0 && is_number) {
            if(ast->name[i] < '0' || ast->name[i] > '9') {
                is_number = false;
            }
            i++;
        }
        if(is_number) {
            return LLVMIntType(atoi(ast->name + 1));
        }
    }
    addError(error_context, "Expected a type", ast->start, ERROR);
    return NULL;
}

LLVMTypeRef generateTypeReference(AstUnaryOperation* ast, Args* args, ErrorContext* error_context) {
    LLVMTypeRef base = generateType(ast->operand, args, error_context);
    if(base != NULL) {
        return LLVMPointerType(base, 0);
    }
    return NULL;
}

LLVMTypeRef generateTypeAddress(Ast* ast, Args* args, ErrorContext* error_context) {
    return LLVMPointerType(LLVMIntType(8), 0);
}

typedef LLVMTypeRef (*GenerateTypeFunction)(Ast* ast, Args* args, ErrorContext* error_context);

static GenerateTypeFunction generation_functions[] = {
    [AST_ROOT] = (GenerateTypeFunction)NULL,
    [AST_CODE_BLOCK] = (GenerateTypeFunction)NULL,
    [AST_PARAMETER_DEFINITION] = (GenerateTypeFunction)NULL,
    [AST_FUNCTION_DEFINITION] = (GenerateTypeFunction)NULL,
    [AST_VARIABLE_DEFINITION] = (GenerateTypeFunction)NULL,
    [AST_IF_ELSE] = (GenerateTypeFunction)NULL,
    [AST_FOR_LOOP] = (GenerateTypeFunction)NULL,
    [AST_INTEGER_LITERAL] = (GenerateTypeFunction)NULL,
    [AST_FLOAT_LITERAL] = (GenerateTypeFunction)NULL,
    [AST_VARIABLE_ACCESS] = (GenerateTypeFunction)generateTypeBase,
    [AST_CALL] = (GenerateTypeFunction)NULL,
    [AST_INDEX] = (GenerateTypeFunction)NULL,

    // Assignments
    [AST_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_OR_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_AND_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_XOR_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_SHIFT_RIGHT_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_SHIFT_LEFT_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_ADD_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_SUBTRACT_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_MULTIPLY_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_DIVIDE_ASSIGNMENT] = (GenerateTypeFunction)NULL,
    [AST_REMAINDER_ASSIGNMENT] = (GenerateTypeFunction)NULL,

    // Binary operation
    [AST_LAZY_AND] = (GenerateTypeFunction)NULL,
    [AST_LAZY_OR] = (GenerateTypeFunction)NULL,
    [AST_EQUAL] = (GenerateTypeFunction)NULL,
    [AST_UNEQUAL] = (GenerateTypeFunction)NULL,
    [AST_GREATER_EQUAL] = (GenerateTypeFunction)NULL,
    [AST_LESS_EQUAL] = (GenerateTypeFunction)NULL,
    [AST_GREATER] = (GenerateTypeFunction)NULL,
    [AST_LESS] = (GenerateTypeFunction)NULL,
    [AST_OR] = (GenerateTypeFunction)NULL,
    [AST_AND] = (GenerateTypeFunction)NULL,
    [AST_XOR] = (GenerateTypeFunction)NULL,
    [AST_SHIFT_RIGHT] = (GenerateTypeFunction)NULL,
    [AST_SHIFT_LEFT] = (GenerateTypeFunction)NULL,
    [AST_ADD] = (GenerateTypeFunction)NULL,
    [AST_SUBTRACT] = (GenerateTypeFunction)NULL,
    [AST_MULTIPLY] = (GenerateTypeFunction)NULL,
    [AST_DIVIDE] = (GenerateTypeFunction)NULL,
    [AST_REMAINDER] = (GenerateTypeFunction)NULL,
    [AST_ARRAY] = (GenerateTypeFunction)NULL,

    // Unary operation
    [AST_RETURN] = (GenerateTypeFunction)NULL,
    [AST_DEREFERENCE] = (GenerateTypeFunction)NULL,
    [AST_POSITIVE] = (GenerateTypeFunction)NULL,
    [AST_NEGATIVE] = (GenerateTypeFunction)NULL,
    [AST_REFERENCE] = (GenerateTypeFunction)generateTypeReference,
    [AST_NOT] = (GenerateTypeFunction)NULL,
    [AST_INCREMENT] = (GenerateTypeFunction)NULL,
    [AST_DECREMENT] = (GenerateTypeFunction)NULL,

    // Simple Ast
    [AST_ADDRESS] = (GenerateTypeFunction)generateTypeAddress,
};

LLVMTypeRef generateType(Ast* ast, Args* args, ErrorContext* error_context) {
    if(ast == NULL) {
        return NULL;
    }
    GenerateTypeFunction generation_function = generation_functions[ast->type];
    if(generation_function == NULL) {
        addError(error_context, "Expected a type", ast->start, ERROR);
        return NULL;
    } else {
        return generation_function(ast, args, error_context);
    }
}

LLVMMetadataRef generateTypeMeta(LLVMDIBuilderRef dibuilder, LLVMTypeRef type, LLVMMetadataRef file) {
    if(type == NULL) {
        return NULL;
    }
    switch (LLVMGetTypeKind(type)) {
    case LLVMPointerTypeKind: {
        LLVMMetadataRef base = generateTypeMeta(dibuilder, LLVMGetElementType(type), file);
        return LLVMDIBuilderCreatePointerType(dibuilder, base, 64, 0, 0, NULL, 0);
    }   
    case LLVMArrayTypeKind: {
        LLVMMetadataRef base = generateTypeMeta(dibuilder, LLVMGetElementType(type), file);
        LLVMMetadataRef size = LLVMDIBuilderGetOrCreateSubrange(dibuilder, 0, LLVMGetArrayLength(type));
        return LLVMDIBuilderCreateArrayType(dibuilder, LLVMGetArrayLength(type) * LLVMDITypeGetSizeInBits(base), 0, base, &size, 1);
    }
    case LLVMIntegerTypeKind: {
        int len = LLVMGetIntTypeWidth(type);
        char name[12];
        int size = sprintf(name, "i%i", len);
        return LLVMDIBuilderCreateBasicType(dibuilder, name, size, len, 5, 0);
    }
    case LLVMHalfTypeKind:{
        return LLVMDIBuilderCreateBasicType(dibuilder, "f16", 4, 16, 4, 0);
    }
    case LLVMFloatTypeKind:{
        return LLVMDIBuilderCreateBasicType(dibuilder, "f32", 4, 32, 4, 0);
    }
    case LLVMDoubleTypeKind:{
        return LLVMDIBuilderCreateBasicType(dibuilder, "f64", 4, 64, 4, 0);
    }
    case LLVMX86_FP80TypeKind:{
        return LLVMDIBuilderCreateBasicType(dibuilder, "f80", 4, 80, 4, 0);
    }
    case LLVMFP128TypeKind: {
        return LLVMDIBuilderCreateBasicType(dibuilder, "f128", 4, 128, 4, 0);
    }
    case LLVMFunctionTypeKind: {
        int paramc = 1 + LLVMCountParamTypes(type);
        LLVMMetadataRef paramts[paramc];
        LLVMTypeRef params[paramc];
        LLVMGetParamTypes(type, params);
        paramts[0] = generateTypeMeta(dibuilder, LLVMGetReturnType(type), file);
        for(int i = 0; i < paramc-1; i++) {
            paramts[1 + i] = generateTypeMeta(dibuilder, params[i], file);
        }
        return LLVMDIBuilderCreateSubroutineType(dibuilder, file, paramts, paramc, 0);
    }
    default:
        return NULL;
    }
}
