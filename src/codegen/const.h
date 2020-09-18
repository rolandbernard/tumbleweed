#ifndef _CONST_H_
#define _cONST_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "codegen/symbols.h"

LLVMValueRef generateConstLazyAnd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstLazyOr(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstUnequal(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstGreaterEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstLessEqual(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstGreater(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstLess(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstOr(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstAnd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstXor(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstShiftRight(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstShiftLeft(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstAdd(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstSubtract(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstMultiply(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstDivide(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstRemainder(AstBinaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstReference(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstDereference(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstPositive(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstNegative(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstNot(AstUnaryOperation* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConst(Ast* ast, Args* args, SymbolTable* symbols, ErrorContext* error_context);
            
#endif