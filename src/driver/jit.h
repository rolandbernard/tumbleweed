#ifndef _JIT_H_
#define _JIT_H_

#include <llvm-c/Types.h>

#include "driver/args.h"
#include "driver/error.h"

void runModuleInJIT(LLVMModuleRef module, Args* args, ErrorContext* error_context);

#endif