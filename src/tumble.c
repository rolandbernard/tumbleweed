
#include <stdlib.h>
#include <stdio.h>
#include <llvm-c/Core.h>

#include "driver/file.h"
#include "driver/error.h"
#include "parser/parser.h"
#include "parser/ast-printer.h"
#include "driver/args.h"
#include "driver/compile.h"

int main(int argc, char** argv) {
    int ret = EXIT_SUCCESS;

    ErrorContext errors;
    initErrorContext(&errors);
    Args args;
    parseArgs(argc, argv, &args, &errors);
    FileSet fs;
    initFileSet(&fs);
    if(args.help) {
        printHelpText(stderr, argc, argv);
    } else if(args.input_file_count > 0) {
        compile(&args, &errors, &fs);

        // TODO: This is only here for debunging
        if(getErrorCount(&errors) == 0) {
            FILE* output = fopen(args.output_file != NULL ? args.output_file : "test.out", "w");
            for (int i = 0; i < fs.file_count; i++) {
                printAst(output, fs.files[i].ast, 0);
            }
            fclose(output);
        }
        //

        if(getErrorCount(&errors) == 0) {
            ret = EXIT_FAILURE;
        }
    }
    printErrors(stderr, &errors, &fs);
    freeFileSet(&fs);
    freeArgs(&args);
    freeErrorContext(&errors);

    return ret;
}