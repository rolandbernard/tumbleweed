#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "codegen/symbols.h"

LLVMValueRef generateValueLazyAnd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueLazyOr(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueUnequal(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueGreaterEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueLessEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueGreater(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueLess(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueOr(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueAnd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueXor(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueShiftRight(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueShiftLeft(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueAdd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueSubtract(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueMultiply(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDivide(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueRemainder(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueReference(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDereference(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValuePositive(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueNegative(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueNot(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueBoolNot(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIncrement(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueDecrement(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

#endif