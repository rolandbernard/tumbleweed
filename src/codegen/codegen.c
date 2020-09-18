
#include <string.h>

#include "codegen/codegen.h"

void initCodegen() {
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
}

void deinitCodegen() {
    LLVMShutdown();
}

LLVMModuleRef generateModuleFromAst(Ast* ast, File* file, Args* args, ErrorContext* error_context) {
    LLVMModuleRef module = LLVMModuleCreateWithName(file->path);
    LLVMSetSourceFileName(module, file->filename, strlen(file->filename));
    
    return module;
}
