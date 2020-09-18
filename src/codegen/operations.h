#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "common/hashtable.h"

LLVMValueRef generateValueLazyAnd(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueLazyOr(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueEqual(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueUnequal(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueGreaterEqual(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueLessEqual(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueGreater(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueLess(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueOr(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueAnd(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueXor(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueShiftRight(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueShiftLeft(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueAdd(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueSubtract(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueMultiply(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDivide(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueRemainder(AstBinaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueReference(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDereference(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValuePositive(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueNegative(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueNot(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIncrement(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDecrement(AstUnaryOperation* ast, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

#endif