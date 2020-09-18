#ifndef _ASSIGNMENTS_H_
#define _ASSIGNMENTS_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "common/hashtable.h"

LLVMValueRef generateValueAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueOrAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueAndAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueXorAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueShiftRightAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueShiftLeftAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueAddAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueSubtractAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueMultiplyAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDivideAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueRemainderAssignment(AstAssignment* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

#endif