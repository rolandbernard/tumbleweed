#ifndef _CONST_H_
#define _CONST_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "codegen/symbols.h"

LLVMValueRef generateConstLazyAnd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstLazyOr(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstUnequal(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstGreaterEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstLessEqual(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstGreater(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstLess(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstOr(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstAnd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstXor(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstShiftRight(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstShiftLeft(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstAdd(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstSubtract(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstMultiply(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstDivide(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstRemainder(AstBinaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstPositive(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstNegative(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstNot(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstBoolNot(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstVariableAccess(AstVariableAccess* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConst(Ast* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);
            
#endif