#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"

typedef struct {
    const char* name;
    Ast* ast;
    LLVMValueRef llvm_value;
} Symbol;

#endif