
#include <stdlib.h>
#include <stdio.h>
#include <llvm-c/Core.h>

#include "driver/file.h"
#include "driver/error.h"
#include "parser/parser.h"
#include "parser/ast-printer.h"
#include "driver/args.h"
#include "driver/compile.h"

int main(int argc, const char* const* argv, const char* const* env) {
    int ret = EXIT_SUCCESS;

    ErrorContext errors;
    initErrorContext(&errors);
    Args args;
    parseArgs(argc, argv, env, &args, &errors);
    FileSet fs;
    initFileSet(&fs);
    if(args.help) {
        printHelpText(stderr, argc, argv);
    } else if(args.version) {
        fprintf(stderr, "Version: %s\n", COMPILER_NAME);
    } else if(args.input_file_count > 0) {
        ret = compile(&args, &errors, &fs);
    }
    printErrors(stderr, &errors, &fs);
    freeFileSet(&fs);
    freeArgs(&args);
    freeErrorContext(&errors);

    return ret;
}
