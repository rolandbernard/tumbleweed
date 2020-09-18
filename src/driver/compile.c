
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "driver/compile.h"
#include "parser/parser.h"
#include "codegen/codegen.h"

static char* findPathForImport(Args* args, const char* name) {
    if(access(name, R_OK) == 0) {
        return reducePath(name);
    }
    return NULL;
}

void compile(Args* args, ErrorContext* error_context, FileSet* file_set) {
    initCodegen();
    for (int i = 0; i < args->input_file_count; i++) {
        // TODO: Fillter for the correct extention
        char* path = findPathForImport(args, args->input_files[i]);
        if (path != NULL && getFile(file_set, path) == NULL) {
            File* file = addFile(file_set, path, args->input_files[i]);
            file->ast = parseFile(file, error_context);
        } else if (path == NULL) {
            addErrorf(error_context, NOPOS, ERROR, "%s: File can not be found", args->input_files[i]);
        }
        free(path);
    }
    for (int i = 0; i < file_set->file_count; i++) {
        File* file = &file_set->files[i];
        if(file != NULL && file != PARSE_ERROR) {
            LLVMModuleRef module = generateModuleFromAst(file->ast, file, args, error_context);
            if(module != NULL) {
                LLVMPrintModuleToFile(module, "test.ll", NULL);
                LLVMDisposeModule(module);
            }
        }
    }
    deinitCodegen();
}