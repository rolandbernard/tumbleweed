#ifndef _CASTS_H_
#define _CASTS_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "common/hashtable.h"

LLVMValueRef generateCastFromTo(LLVMValueRef value, LLVMTypeRef dest, LLVMValueRef function, LLVMBuilderRef builder, Args* args, HashTable* symbols, ErrorContext* error_context);

#endif