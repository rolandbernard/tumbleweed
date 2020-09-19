
#include <stdlib.h>
#include <string.h>

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
        symbol->llvm_return = NULL;
        addSymbol(symbols, (void*)symbol);
    }
    free(parameter_types);
    return !error;
}

bool generateFunctionBody(AstFunctionDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    Symbol* symbol = (Symbol*)getSymbol(symbols, ast->name);
    if(symbol == NULL || symbol->ast != (Ast*)ast || symbol->llvm_value == NULL) {
        return false;
    } else {
        if(ast->body != NULL) {
            LLVMBasicBlockRef entry = LLVMAppendBasicBlock(symbol->llvm_value, "");
            LLVMBasicBlockRef block = LLVMAppendBasicBlock(symbol->llvm_value, "");
            LLVMBasicBlockRef exit = LLVMAppendBasicBlock(symbol->llvm_value, "");
            LLVMPositionBuilderAtEnd(builder, entry);
            if(ast->return_type != NULL) {
                symbol->llvm_return = LLVMBuildAlloca(builder, LLVMGetReturnType(LLVMTypeOf(symbol->llvm_value)), "");
            }
            LLVMPositionBuilderAtEnd(builder, block);
            bool ret = generateValueCodeBlock(ast->body, symbol, builder, args, symbols, error_context) != NULL;
            LLVMValueRef last_instr = LLVMGetLastInstruction(LLVMGetInsertBlock(builder));
            if(last_instr == NULL || LLVMGetInstructionOpcode(last_instr) != LLVMBr) {
                LLVMBuildBr(builder, exit);
            }
            LLVMPositionBuilderAtEnd(builder, entry);
            LLVMBuildBr(builder, block);
            LLVMPositionBuilderAtEnd(builder, exit);
            if(ast->return_type != NULL) {
                LLVMBuildRet(builder, symbol->llvm_return);
            } else {
                LLVMBuildRetVoid(builder);
            }
            return ret;
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
    bool error = false;
    appendScope(symbols);
    toNextScope(symbols);
    for (int i = 0; i < ast->children_count; i++) {
        Ast* child = ast->children[i];
        if (child->type == AST_FUNCTION_DEFINITION) {
            if (!generateFunctionShell((AstFunctionDefinition*)child, module, builder, args, symbols, error_context)) {
                error = true;
            }
        }
    }
    for (int i = 0; i < ast->children_count; i++) {
        Ast* child = ast->children[i];
        if (child->type == AST_FUNCTION_DEFINITION) {
            if (!generateFunctionBody((AstFunctionDefinition*)child, module, builder, args, symbols, error_context)) {
                error = true;
            }
        } else if (child->type == AST_VARIABLE_DEFINITION) {
            if (!generateGlobalVariable((AstVariableDefinition*)child, module, builder, args, symbols, error_context)) {
                error = true;
            }
        } else {
            addError(error_context, "Unexpected expression", child->start, ERROR);
            return false;
        }
    }
    toPrevScope(symbols);
    freeLowerScopes(symbols);
    return !error;
}

LLVMValueRef generateValueVariable(AstVariableDefinition* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
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
            LLVMBasicBlockRef current_block = LLVMGetInsertBlock(builder);
            LLVMPositionBuilderAtEnd(builder, LLVMGetEntryBasicBlock(function->llvm_value));
            LLVMValueRef variable = LLVMBuildAlloca(builder, variable_type, "");
            LLVMPositionBuilderAtEnd(builder, current_block);
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

LLVMValueRef generateValueIfElse(AstIfElse* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
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
            condition_bool = LLVMBuildICmp(builder, LLVMIntNE, condition, LLVMConstNull(condition_type), "");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_bool = LLVMBuildFCmp(builder, LLVMRealONE, condition, LLVMConstNull(condition_type), "");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->condition->start, ERROR);
            error = true;
            break;
        }
    }
    LLVMBasicBlockRef if_start = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(LLVMGetInsertBlock(builder)), "");
    LLVMBasicBlockRef else_start = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(if_start), "");
    LLVMBasicBlockRef ifelse_end = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(else_start), "");
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
    bool require_end = false;
    LLVMValueRef last_instr = LLVMGetLastInstruction(LLVMGetInsertBlock(builder));
    if(last_instr == NULL || LLVMGetInstructionOpcode(last_instr) != LLVMBr) {
        LLVMBuildBr(builder, ifelse_end);
        require_end = true;   
    }
    LLVMPositionBuilderAtEnd(builder, else_start);
    if (ast->else_block != NULL) {
        LLVMValueRef else_value = generateValueInFunction(ast->else_block, function, builder, args, symbols, error_context);
        if (else_value == NULL) {
            error = true;
        }
    }
    last_instr = LLVMGetLastInstruction(LLVMGetInsertBlock(builder));
    if(last_instr == NULL || LLVMGetInstructionOpcode(last_instr) != LLVMBr) {
        LLVMBuildBr(builder, ifelse_end);
        require_end = true;   
    }
    if(require_end) {
        LLVMPositionBuilderAtEnd(builder, ifelse_end);
    } else {
        LLVMDeleteBasicBlock(ifelse_end);
    }
    if(error) {
        return NULL;
    } else {
        return (LLVMValueRef)1;
    }
}

LLVMValueRef generateValueForLoop(AstForLoop* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMBasicBlockRef for_start = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(LLVMGetInsertBlock(builder)), "");
    LLVMBasicBlockRef for_body = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(for_start), "");
    LLVMBasicBlockRef for_end = LLVMInsertBasicBlock(LLVMGetNextBasicBlock(for_body), "");
    LLVMBuildBr(builder, for_start);
    LLVMPositionBuilderAtEnd(builder, for_start);
    LLVMValueRef condition = generateValueInFunction(ast->condition, function, builder, args, symbols, error_context);
    LLVMValueRef condition_bool = NULL;
    if (condition == NULL) {
        error = true;
    } else {
        LLVMTypeRef condition_type = LLVMTypeOf(condition);
        switch (LLVMGetTypeKind(condition_type)) {
        case LLVMIntegerTypeKind:
        case LLVMPointerTypeKind:
            condition_bool = LLVMBuildICmp(builder, LLVMIntNE, condition, LLVMConstNull(condition_type), "");
            break;
        case LLVMHalfTypeKind:
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
        case LLVMX86_FP80TypeKind:
        case LLVMFP128TypeKind:
            condition_bool = LLVMBuildFCmp(builder, LLVMRealONE, condition, LLVMConstNull(condition_type), "");
            break;
        default:
            addError(error_context, "Condition has a unsupported type", ast->condition->start, ERROR);
            error = true;
            break;
        }
    }
    if(condition_bool != NULL) {
        LLVMBuildCondBr(builder, condition_bool, for_body, for_end);
    }
    LLVMPositionBuilderAtEnd(builder, for_body);
    LLVMValueRef body = generateValueInFunction(ast->code_block, function, builder, args, symbols, error_context);
    if(body == NULL) {
        error = true;
    }
    LLVMValueRef last_instr = LLVMGetLastInstruction(LLVMGetInsertBlock(builder));
    if(last_instr == NULL || LLVMGetInstructionOpcode(last_instr) != LLVMBr) {
        LLVMBuildBr(builder, for_start);
    }
    LLVMPositionBuilderAtEnd(builder, for_end);
    if(error) {
        return NULL;
    } else {
        return (LLVMValueRef)1;
    }
}

LLVMValueRef generateValueReturn(AstUnaryOperation* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    if(ast->operand != NULL) {
        if(function->llvm_return == NULL) {
            addError(error_context, "This function can't return a value", ast->operand->start, ERROR);
            return NULL;
        } else {
            LLVMValueRef value = generateValueInFunction(ast->operand, function, builder, args, symbols, error_context);
            if(value != NULL) {
                LLVMBuildStore(builder, value, function->llvm_return);
            }
            LLVMBasicBlockRef exit = LLVMGetLastBasicBlock(function->llvm_value);
            LLVMBuildBr(builder, exit);
            return value;
        }
    } else {
        if(function->llvm_return != NULL) {
            addError(error_context, "This function has to return a value", ast->start, ERROR);
            return NULL;
        } else {
            LLVMBasicBlockRef exit = LLVMGetLastBasicBlock(function->llvm_value);
            LLVMBuildBr(builder, exit);
            return (LLVMValueRef)1;
        }
    }
}

LLVMValueRef generateValueCodeBlock(AstCodeBlock* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    appendScope(symbols);
    toNextScope(symbols);
    for(int i = 0; i < ast->children_count; i++) {
        LLVMValueRef value = generateValueInFunction(ast->children[i], function, builder, args, symbols, error_context);
        if(value == NULL) {
            error = true;
        }
    }
    toPrevScope(symbols);
    freeLowerScopes(symbols);
    if(error) {
        return NULL;
    } else {
        return (LLVMValueRef)1;
    }
}

LLVMValueRef generateValueIntegerLiteral(AstIntegerLiteral* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
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

LLVMValueRef generateValueFloatLiteral(AstFloatLiteral* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    return LLVMConstRealOfString(LLVMDoubleType(), ast->float_string);
}

LLVMValueRef generateValueVariableAccess(AstVariableAccess* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    Symbol* symbol = getSymbol(symbols, ast->name);
    if(symbol == NULL) {
        addErrorf(error_context, ast->start, ERROR, "Undefined symbol '%s'", ast->name);
        return NULL;
    } else {
        if(symbol->ast->type == AST_VARIABLE_DEFINITION) {
            return LLVMBuildLoad2(builder, LLVMGetElementType(LLVMTypeOf(symbol->llvm_value)), symbol->llvm_value, "");
        } else {
            return symbol->llvm_value;
        }
    }
}

LLVMValueRef generateValueCall(AstCall* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef func = generateValueInFunction(ast->to_call, function, builder, args, symbols, error_context);
    if(func == NULL) {
        error = true;
    }
    if(!LLVMIsAFunction(func) &&
        !(LLVMGetTypeKind(LLVMTypeOf(func)) == LLVMPointerTypeKind && LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(func))) == LLVMFunctionTypeKind)) {
        addError(error_context, "Only functions can be called", ast->to_call->start, ERROR);
        error = true;
    }
    LLVMValueRef* params = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * ast->parameter_count);
    for(int i = 0; i < ast->parameter_count; i++) {
        params[i] = generateValueInFunction(ast->parameters[i], function, builder, args, symbols, error_context);
        if(params[i] == NULL) {
            error = true;
        }
    }
    if(!error) {
        if(LLVMCountParams(func) == ast->parameter_count) {
            LLVMTypeRef* param_types = (LLVMTypeRef*)malloc(sizeof(LLVMTypeRef) * ast->parameter_count);
            LLVMGetParamTypes(LLVMGetElementType(LLVMTypeOf(func)), param_types);
            for(int i = 0; i < ast->parameter_count; i++) {
                params[i] = generateCastFromTo(params[i], param_types[i], function, builder, args, symbols, error_context);
                if(params[i] == NULL) {
                    error = true;
                }
            }
            free(param_types);
            if(!error) {
                LLVMValueRef ret = LLVMBuildCall(builder, func, params, ast->parameter_count, "");
                free(params);
                return ret;
            }
        } else {
            addErrorf(error_context, ast->start, ERROR, "The function takes %i params but was called with %i",
                LLVMCountParams(func), ast->parameter_count);
            error = true;
        }
    }
    free(params);
    return NULL;
}

LLVMValueRef generateValueIndex(AstIndex* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
    bool error = false;
    LLVMValueRef pointer = generateValueInFunction(ast->pointer, function, builder, args, symbols, error_context);
    if(pointer == NULL) {
        error = true;
    }
    if(LLVMIsAFunction(pointer) ||
        (LLVMGetTypeKind(LLVMTypeOf(pointer)) == LLVMPointerTypeKind && LLVMGetTypeKind(LLVMGetElementType(LLVMTypeOf(pointer))) == LLVMFunctionTypeKind) || 
        LLVMGetTypeKind(LLVMTypeOf(pointer)) != LLVMPointerTypeKind) {
        addError(error_context, "Only pointers can be indexed", ast->pointer->start, ERROR);
        error = true;
    }
    LLVMValueRef index = generateValueInFunction(ast->index, function, builder, args, symbols, error_context);
    if(index == NULL) {
        error = true;
    }
    if(LLVMGetTypeKind(LLVMTypeOf(index)) != LLVMIntegerTypeKind) {
        addError(error_context, "Index must be a integer", ast->index->start, ERROR);
        error = true;
    }
    if(!error) {
    }
    return NULL;
}

typedef LLVMValueRef (*GenerateValueFunction)(Ast* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

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

LLVMValueRef generateValueInFunction(Ast* ast, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context) {
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
