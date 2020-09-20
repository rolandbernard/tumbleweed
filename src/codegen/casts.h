#ifndef _CASTS_H_
#define _CASTS_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "codegen/symbols.h"

LLVMValueRef generateCastFromTo(Ast* ast, LLVMValueRef value, LLVMTypeRef dest, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstCastFromTo(Ast* ast, LLVMValueRef value, LLVMTypeRef dest, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateExtractFromVariable(LLVMValueRef value, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);
    
#endif