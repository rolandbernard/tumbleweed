#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"

LLVMModuleRef* generateModuleFromAst(Ast* ast);

#endif