#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/file.h"
#include "driver/error.h"

void initCodegen();

void deinitCodegen();

LLVMModuleRef generateModuleFromAst(Ast* ast, File* file, Args* args, ErrorContext* error_context);

#endif