#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "common/hashtable.h"

LLVMValueRef generateValueRoot(AstRoot* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueFunctionShell(AstFunctionDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueFunctionBody(AstFunctionDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueGlobalVariable(AstVariableDefinition* ast, LLVMModuleRef module, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueVariable(AstIfElse* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIfElse(AstIfElse* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueForLoop(AstForLoop* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueReturn(AstForLoop* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueCodeBlock(AstCodeBlock* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIntegerLiteral(AstIntegerLiteral* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueFloatLiteral(AstFloatLiteral* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueVariableAccess(AstVariableAccess* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueCall(AstCall* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIndex(AstIndex* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueInFunction(Ast* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

#endif