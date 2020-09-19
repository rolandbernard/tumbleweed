#ifndef _CASTS_H_
#define _CASTS_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "codegen/symbols.h"

LLVMValueRef generateCastFromTo(LLVMValueRef value, LLVMTypeRef dest, Symbol* function, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateConstCastFromTo(LLVMValueRef value, LLVMTypeRef dest, Args* args, SymbolTable* symbols, ErrorContext* error_context);

#endif