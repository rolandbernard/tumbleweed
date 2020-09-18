
#include <string.h>

#include "codegen/codegen.h"
#include "codegen/general.h"
#include "codegen/symbols.h"

void initCodegen() {
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
}

void deinitCodegen() {
    LLVMShutdown();
}

LLVMModuleRef generateModuleFromAst(Ast* ast, File* file, Args* args, ErrorContext* error_context) {
    if(ast != NULL && ast->type == AST_ROOT) {
        LLVMModuleRef module = LLVMModuleCreateWithName(file->path);
        LLVMSetSourceFileName(module, file->filename, strlen(file->filename));
        LLVMBuilderRef builder = LLVMCreateBuilder();
        SymbolTable symbols;
        initSymbolTable(&symbols);
        if (!generateRoot((AstRoot*)ast, module, builder, args, &symbols, error_context)) {
            freeSymbolTable(&symbols);
            LLVMDisposeModule(module);
            LLVMDisposeBuilder(builder);
            return NULL;
        } else {
            freeSymbolTable(&symbols);
            LLVMDisposeBuilder(builder);
            return module;
        }
    } else {
        return NULL;
    }
}
