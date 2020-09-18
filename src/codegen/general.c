
#include <stdlib.h>

#include "codegen/general.h"
#include "codegen/assignments.h"
#include "codegen/operations.h"
#include "codegen/types.h"
#include "codegen/symbols.h"
#include "codegen/const.h"
#include "codegen/casts.h"

bool generateFunctionShell(AstFunctionDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMTypeRef return_type;
    if (ast->return_type == NULL) {
        return_type = LLVMVoidType();
    } else {
        return_type = generateType(ast->return_type, args, error_context);
        if (return_type == NULL) {
            error = true;
        }
    }
    LLVMTypeRef* parameter_types = (LLVMTypeRef*)malloc(sizeof(LLVMTypeRef) * ast->parameter_count);
    for (int i = 0; i < ast->parameter_count; i++) {
        parameter_types[i] = generateType(ast->parameters[i]->parameter_type, args, error_context);
        if (parameter_types[i] == NULL) {
            error = true;
        }
    }
    if (getSymbol(symbols, ast->name) != NULL) {
        Symbol* old_symbol = (Symbol*)getSymbol(symbols, ast->name);
        addErrorf(error_context, ast->start, ERROR, "Duplicate symbol name '%s'", ast->name);
        addErrorf(error_context, old_symbol->ast->start, NOTE, "Previous definition of '%s'", ast->name);
        error = true;
    } else {
        Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
        symbol->name = ast->name;
        symbol->ast = (Ast*)ast;
        if (!error) {
            LLVMTypeRef function_type = LLVMFunctionType(return_type, parameter_types, ast->parameter_count, false);
            LLVMValueRef function = LLVMAddFunction(module, ast->name, function_type);
            symbol->llvm_value = function;
        } else {
            symbol->llvm_value = NULL;
        }
        addSymbol(symbols, (void*)symbol);
    }
    free(parameter_types);
    return !error;
}

bool generateFunctionBody(AstFunctionDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    Symbol* symbol = (Symbol*)getSymbol(symbols, ast->name);
    if(symbol == NULL || symbol->ast != (Ast*)ast) {
        return false;
    } else {
        if(ast->body != NULL) {
            LLVMAppendBasicBlock(symbol->llvm_value, "entry");
            LLVMBasicBlockRef block = LLVMAppendBasicBlock(symbol->llvm_value, "after_variables");
            LLVMPositionBuilderAtEnd(builder, block);
            return generateValueCodeBlock(ast->body, symbol->llvm_value, builder, args, symbols, error_context) != NULL;
        } else {
            return true;
        }
    }
}

bool generateGlobalVariable(AstVariableDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef value;
    if (ast->initial_value != NULL) {
        if (ast->is_extern) {
            addError(error_context, "Extern values can't be initialized", ast->start, ERROR);
        } else {
            value = generateConst(ast->initial_value, args, symbols, error_context);
            if (value == NULL) {
                error = true;
            }
        }
    }
    LLVMTypeRef variable_type;
    if (ast->variable_type == NULL) {
        variable_type = LLVMTypeOf(value);
    } else {
        if (ast->variable_type->type == AST_ARRAY) {
            if (ast->initial_value != NULL) {
                addError(error_context, "Arrays can't be initialized", ast->initial_value->start, ERROR);
                error = true;
            }
            AstBinaryOperation* ast_array = (AstBinaryOperation*)ast->variable_type;
            variable_type = generateType(ast_array->rhs, args, error_context);
            if (variable_type == NULL) {
                error = true;
            } else {
                LLVMValueRef size = generateConst(ast->initial_value, args, symbols, error_context);
                if (size == NULL) {
                    error = true;
                } else {
                    if (!(LLVMGetTypeKind(LLVMTypeOf(size)) != LLVMIntegerTypeKind)) {
                        addError(error_context, "The Array size must be an integer", ast_array->lhs->start, ERROR);
                        error = true;
                    } else {
                        long long size_int = LLVMConstIntGetSExtValue(size);
                        if (size_int < 0) {
                            addError(error_context, "Sizes can't be negative", ast_array->lhs->start, ERROR);
                            error = true;
                        } else {
                            variable_type = LLVMArrayType(variable_type, size_int);
                        }
                    }
                }
            }
        } else {
            variable_type = generateType(ast->variable_type, args, error_context);
            if (variable_type == NULL) {
                error = true;
            } else if (ast->initial_value != NULL) {
                value = generateConstCastFromTo(value, variable_type, args, symbols, error_context);
                if(value == NULL) {
                    error = true;
                }
            }
        }
    }
    if (getSymbol(symbols, ast->name) != NULL) {
        Symbol* old_symbol = (Symbol*)getSymbol(symbols, ast->name);
        addErrorf(error_context, ast->start, ERROR, "Duplicate symbol name '%s'", ast->name);
        addErrorf(error_context, old_symbol->ast->start, NOTE, "Previous definition of '%s'", ast->name);
        error = true;
    } else {
        Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
        symbol->name = ast->name;
        symbol->ast = (Ast*)ast;
        if (!error) {
            LLVMValueRef variable = LLVMAddGlobal(module, variable_type, ast->name);
            if (ast->initial_value != NULL) {
                LLVMSetInitializer(variable, value);
            }
            symbol->llvm_value = variable;
        } else {
            symbol->llvm_value = NULL;
        }
        addSymbol(symbols, (void*)symbol);
    }
    return !error;
}

bool generateRoot(AstRoot* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    appendScope(symbols);
    toNextScope(symbols);
    for (int i = 0; i < ast->children_count; i++) {
        Ast* child = ast->children[i];
        if (child->type == AST_FUNCTION_DEFINITION) {
            if (!generateFunctionShell((AstFunctionDefinition*)child, module, builder, args, symbols, error_context)) {
                return false;
            }
        }
    }
    for (int i = 0; i < ast->children_count; i++) {
        Ast* child = ast->children[i];
        if (child->type == AST_FUNCTION_DEFINITION) {
            if (!generateFunctionBody((AstFunctionDefinition*)child, module, builder, args, symbols, error_context)) {
                return false;
            }
        } else if (child->type == AST_VARIABLE_DEFINITION) {
            if (!generateGlobalVariable((AstVariableDefinition*)child, module, builder, args, symbols, error_context)) {
                return false;
            }
        } else {
            addError(error_context, "Unexpected expression", child->start, ERROR);
            return false;
        }
    }
    toPrevScope(symbols);
    freeLowerScopes(symbols);
    return true;
}

LLVMValueRef generateValueVariable(AstVariableDefinition* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef value;
    if (ast->initial_value != NULL) {
        if (ast->is_extern) {
            addError(error_context, "Extern values can't be initialized", ast->start, ERROR);
        } else {
            value = generateValueInFunction(ast->initial_value, function, builder, args, symbols, error_context);
            if (value == NULL) {
                error = true;
            }
        }
    }
    LLVMTypeRef variable_type;
    if (ast->variable_type == NULL) {
        variable_type = LLVMTypeOf(value);
    } else {
        if (ast->variable_type->type == AST_ARRAY) {
            if (ast->initial_value != NULL) {
                addError(error_context, "Arrays can't be initialized", ast->initial_value->start, ERROR);
                error = true;
            }
            AstBinaryOperation* ast_array = (AstBinaryOperation*)ast->variable_type;
            variable_type = generateType(ast_array->rhs, args, error_context);
            if (variable_type == NULL) {
                error = true;
            } else {
                LLVMValueRef size = generateConst(ast->initial_value, args, symbols, error_context);
                if (size == NULL) {
                    error = true;
                } else {
                    if (!(LLVMGetTypeKind(LLVMTypeOf(size)) != LLVMIntegerTypeKind)) {
                        addError(error_context, "The Array size must be an integer", ast_array->lhs->start, ERROR);
                        error = true;
                    } else {
                        long long size_int = LLVMConstIntGetSExtValue(size);
                        if (size_int < 0) {
                            addError(error_context, "Sizes can't be negative", ast_array->lhs->start, ERROR);
                            error = true;
                        } else {
                            variable_type = LLVMArrayType(variable_type, size_int);
                        }
                    }
                }
            }
        } else {
            variable_type = generateType(ast->variable_type, args, error_context);
            if (variable_type == NULL) {
                error = true;
            } else if (ast->initial_value != NULL) {
                value = generateCastFromTo(value, variable_type, function, builder, args, symbols, error_context);
                if(value == NULL) {
                    error = true;
                }
            }
        }
    }
    if (getSymbolInCurrentScope(symbols, ast->name) != NULL) {
        Symbol* old_symbol = (Symbol*)getSymbol(symbols, ast->name);
        addErrorf(error_context, ast->start, ERROR, "Duplicate symbol name '%s'", ast->name);
        addErrorf(error_context, old_symbol->ast->start, NOTE, "Previous definition of '%s'", ast->name);
        error = true;
    } else {
        Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
        symbol->name = ast->name;
        symbol->ast = (Ast*)ast;
        if (!error) {
            LLVMBuilderRef alloca_builder = LLVMCreateBuilder();
            LLVMPositionBuilderAtEnd(alloca_builder, LLVMGetEntryBasicBlock(function));
            LLVMValueRef variable = LLVMBuildAlloca(alloca_builder, variable_type, ast->name);
            LLVMDisposeBuilder(alloca_builder);
            if (ast->initial_value != NULL) {
                LLVMBuildStore(builder, value, variable);
            }
            symbol->llvm_value = variable;
            addSymbol(symbols, (void*)symbol);
            return variable;
        } else {
            symbol->llvm_value = NULL;
            addSymbol(symbols, (void*)symbol);
        }
    }
    return NULL;
}

LLVMValueRef generateValueIfElse(AstIfElse* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef condition = generateValueInFunction(ast->condition, function, builder, args, symbols, error_context);
    LLVMValueRef condition_bool = NULL;
    if (condition == NULL) {
        error = true;
    } else {
        LLVMTypeRef condition_type = LLVMTypeOf(condition);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
        case LLVMPointerTypeKind:
            condition_bool = LLVMBuildICmp(builder, LLVMIntEQ, condition, LLVMConstNull(condition_type), "condition");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_bool = LLVMBuildFCmp(builder, LLVMRealOEQ, condition, LLVMConstNull(condition_type), "condition");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->condition->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMBasicBlockRef if_start = LLVMAppendBasicBlock(function, "if_start");
    LLVMBasicBlockRef else_start = LLVMAppendBasicBlock(function, "else_start");
    LLVMBasicBlockRef ifelse_end = LLVMAppendBasicBlock(function, "ifelse_end");
    if (condition_bool == NULL) {
        error = true;
    } else {
        LLVMBuildCondBr(builder, condition_bool, if_start, else_start);
    }
    LLVMPositionBuilderAtEnd(builder, if_start);
    LLVMValueRef if_value = generateValueInFunction(ast->if_block, function, builder, args, symbols, error_context);
    if (if_value == NULL) {
        error = true;
    }
    LLVMBuildBr(builder, ifelse_end);
    LLVMPositionBuilderAtEnd(builder, else_start);
    if (ast->else_block != NULL) {
        LLVMValueRef else_value = generateValueInFunction(ast->else_block, function, builder, args, symbols, error_context);
        if (else_value == NULL) {
            error = true;
        }
    }
    LLVMBuildBr(builder, ifelse_end);
    LLVMPositionBuilderAtEnd(builder, ifelse_end);
    if(error) {
        return NULL;
    } else {
        return (LLVMValueRef)1;
    }
}

LLVMValueRef generateValueForLoop(AstForLoop* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueReturn(AstForLoop* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueCodeBlock(AstCodeBlock* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueIntegerLiteral(AstIntegerLiteral* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueFloatLiteral(AstFloatLiteral* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueVariableAccess(AstVariableAccess* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueCall(AstCall* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

LLVMValueRef generateValueIndex(AstIndex* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return NULL;
}

typedef LLVMValueRef (*GenerateValueFunction)(Ast* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

static GenerateValueFunction generation_functions[] = {
    [AST_ROOT] = (GenerateValueFunction)NULL,
    [AST_CODE_BLOCK] = (GenerateValueFunction)generateValueCodeBlock,
    [AST_PARAMETER_DEFINITION] = (GenerateValueFunction)NULL,
    [AST_FUNCTION_DEFINITION] = (GenerateValueFunction)NULL,
    [AST_VARIABLE_DEFINITION] = (GenerateValueFunction)generateValueVariable,
    [AST_IF_ELSE] = (GenerateValueFunction)generateValueIfElse,
    [AST_FOR_LOOP] = (GenerateValueFunction)generateValueForLoop,
    [AST_INTEGER_LITERAL] = (GenerateValueFunction)generateValueIntegerLiteral,
    [AST_FLOAT_LITERAL] = (GenerateValueFunction)generateValueFloatLiteral,
    [AST_VARIABLE_ACCESS] = (GenerateValueFunction)generateValueVariableAccess,
    [AST_CALL] = (GenerateValueFunction)generateValueCall,
    [AST_INDEX] = (GenerateValueFunction)generateValueIndex,

    // Assignments
    [AST_ASSIGNMENT] = (GenerateValueFunction)generateValueAssignment,
    [AST_OR_ASSIGNMENT] = (GenerateValueFunction)generateValueOrAssignment,
    [AST_AND_ASSIGNMENT] = (GenerateValueFunction)generateValueAndAssignment,
    [AST_XOR_ASSIGNMENT] = (GenerateValueFunction)generateValueXorAssignment,
    [AST_SHIFT_RIGHT_ASSIGNMENT] = (GenerateValueFunction)generateValueShiftRightAssignment,
    [AST_SHIFT_LEFT_ASSIGNMENT] = (GenerateValueFunction)generateValueShiftLeftAssignment,
    [AST_ADD_ASSIGNMENT] = (GenerateValueFunction)generateValueAddAssignment,
    [AST_SUBTRACT_ASSIGNMENT] = (GenerateValueFunction)generateValueSubtractAssignment,
    [AST_MULTIPLY_ASSIGNMENT] = (GenerateValueFunction)generateValueMultiplyAssignment,
    [AST_DIVIDE_ASSIGNMENT] = (GenerateValueFunction)generateValueDivideAssignment,
    [AST_REMAINDER_ASSIGNMENT] = (GenerateValueFunction)generateValueRemainderAssignment,

    // Binary operation
    [AST_LAZY_AND] = (GenerateValueFunction)generateValueLazyAnd,
    [AST_LAZY_OR] = (GenerateValueFunction)generateValueLazyOr,
    [AST_EQUAL] = (GenerateValueFunction)generateValueEqual,
    [AST_UNEQUAL] = (GenerateValueFunction)generateValueUnequal,
    [AST_GREATER_EQUAL] = (GenerateValueFunction)generateValueGreaterEqual,
    [AST_LESS_EQUAL] = (GenerateValueFunction)generateValueLessEqual,
    [AST_GREATER] = (GenerateValueFunction)generateValueGreater,
    [AST_LESS] = (GenerateValueFunction)generateValueLess,
    [AST_OR] = (GenerateValueFunction)generateValueOr,
    [AST_AND] = (GenerateValueFunction)generateValueAnd,
    [AST_XOR] = (GenerateValueFunction)generateValueXor,
    [AST_SHIFT_RIGHT] = (GenerateValueFunction)generateValueShiftRight,
    [AST_SHIFT_LEFT] = (GenerateValueFunction)generateValueShiftLeft,
    [AST_ADD] = (GenerateValueFunction)generateValueAdd,
    [AST_SUBTRACT] = (GenerateValueFunction)generateValueSubtract,
    [AST_MULTIPLY] = (GenerateValueFunction)generateValueMultiply,
    [AST_DIVIDE] = (GenerateValueFunction)generateValueDivide,
    [AST_REMAINDER] = (GenerateValueFunction)generateValueRemainder,
    [AST_ARRAY] = (GenerateValueFunction)NULL,

    // Unary operation
    [AST_RETURN] = (GenerateValueFunction)generateValueReturn,
    [AST_DEREFERENCE] = (GenerateValueFunction)generateValueDereference,
    [AST_POSITIVE] = (GenerateValueFunction)generateValuePositive,
    [AST_NEGATIVE] = (GenerateValueFunction)generateValueNegative,
    [AST_REFERENCE] = (GenerateValueFunction)generateValueReference,
    [AST_NOT] = (GenerateValueFunction)generateValueNot,
    [AST_INCREMENT] = (GenerateValueFunction)generateValueIncrement,
    [AST_DECREMENT] = (GenerateValueFunction)generateValueDecrement,

    // Simple Ast
    [AST_ADDRESS] = (GenerateValueFunction)NULL,
};

LLVMValueRef generateValueInFunction(Ast* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    GenerateValueFunction generation_function = generation_functions[ast->type];
    if(generation_function == NULL) {
        switch (ast->type) {
        case AST_ADDRESS:
            addError(error_context, "Did not expect a type", ast->start, ERROR);
            break;
        case AST_ARRAY:
            addError(error_context, "This is not a  definition", ast->start, ERROR);
            break;
        default:
            addError(error_context, "Unexpected expression", ast->start, ERROR);
            break;
        }
        return NULL;
    } else {
        return generation_function(ast, function, builder, args, symbols, error_context);
    }
}
