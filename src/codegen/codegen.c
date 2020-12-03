
#include <string.h>
#include <llvm-c/DebugInfo.h>

#include "codegen/codegen.h"
#include "codegen/general.h"
#include "codegen/symbols.h"

LLVMModuleRef generateModuleFromAst(Ast* ast, File* file, Args* args, ErrorContext* error_context) {
    if(ast != NULL && ast->type == AST_ROOT) {
        LLVMModuleRef module = LLVMModuleCreateWithName(file->filename);
        LLVMSetSourceFileName(module, file->filename, strlen(file->filename));
        LLVMBuilderRef builder = LLVMCreateBuilder();
        LLVMDIBuilderRef dibuilder = NULL;
        LLVMMetadataRef file_meta = NULL;
        if(args->debug) {
            dibuilder = LLVMCreateDIBuilder(module);
            file_meta = LLVMDIBuilderCreateFile(dibuilder, file->path + file->directory_str_len + 1, file->file_str_len, file->path, file->directory_str_len);
            LLVMDIBuilderCreateCompileUnit(dibuilder, LLVMDWARFSourceLanguageC, file_meta, COMPILER_NAME, strlen(COMPILER_NAME), args->size_opt != 0 || args->speed_opt != 0, NULL, 0, 0, NULL, 0, LLVMDWARFEmissionFull, 0, false, false, NULL, 0, NULL, 0);
            LLVMAddModuleFlag(module, LLVMModuleFlagBehaviorWarning, "Debug Info Version", 18, LLVMValueAsMetadata(LLVMConstInt(LLVMIntType(32), LLVMDebugMetadataVersion(), 0)));
        }
        SymbolTable symbols;
        initSymbolTable(&symbols);
        if (!generateRoot((AstRoot*)ast, file, file_meta, module, dibuilder, builder, args, &symbols, error_context)) {
            freeSymbolTable(&symbols);
            LLVMDisposeModule(module);
            LLVMDisposeBuilder(builder);
            if(args->debug) {
                LLVMDIBuilderFinalize(dibuilder);
                LLVMDisposeDIBuilder(dibuilder);
            }
            return NULL;
        } else {
            freeSymbolTable(&symbols);
            LLVMDisposeBuilder(builder);
            if(args->debug) {
                LLVMDIBuilderFinalize(dibuilder);
                LLVMDisposeDIBuilder(dibuilder);
            }
            return module;
        }
    } else {
        return NULL;
    }
}
