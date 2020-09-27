
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <llvm-c/Linker.h>
#include <llvm-c/Transforms/PassManagerBuilder.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/IRReader.h>

#include "driver/compile.h"
#include "parser/parser.h"
#include "codegen/codegen.h"
#include "common/exec.h"
#include "driver/jit.h"
#include "common/arraylist.h"

#define TMP_OBJECT_NAME_PREFIX "/tmp/tumbleweed_tmp_obj."
#define MAX_LINK_ERROR 1000000

static char* findPathForImport(Args* args, const char* name) {
    if(access(name, R_OK) == 0) {
        return reducePath(name);
    }
    return NULL;
}

static void initLLVM() {
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
}

static void deinitLLVM() {
    LLVMShutdown();
}

static char* changeExt(const char* file, const char* new_ext) {
    int len = strlen(file);
    while (len > 0 && file[len] != '.' && file[len] != '/') {
        len--;
    }
    if(len == 0 || file[len] == '/') {
        len = strlen(file);
    }
    int ext_len = strlen(new_ext);
    char* ret = (char*)malloc(len + ext_len + 1);
    memcpy(ret, file, len);
    memcpy(ret + len, new_ext, ext_len);
    ret[len + ext_len] = 0;
    return ret;
}

static bool testExt(const char* file, const char* ext) {
    int len = strlen(file);
    int ext_len = strlen(ext);
    for(int i = 1; i < len && i < ext_len; i++) {
        if(file[len - i] != ext[ext_len - i]) {
            return false;
        }
    }
    return true;
}

int compile(Args* args, ErrorContext* error_context, FileSet* file_set) {
    int ret = 0;
    initLLVM();
    char* error_msg;
    for (int i = 0; i < args->input_file_count; i++) {
        if (testExt(args->input_files[i], ".tumble")) {
            char* path = findPathForImport(args, args->input_files[i]);
            if (path != NULL && getFile(file_set, path) == NULL) {
                File* file = addFile(file_set, path, args->input_files[i]);
                file->ast = parseFile(file, error_context);
                if (file->ast == PARSE_ERROR) {
                    file->ast = NULL;
                }
            } else if (path == NULL) {
                addErrorf(error_context, NOPOS, ERROR, "%s: File can not be found", args->input_files[i]);
            }
            free(path);
        } else if ((testExt(args->input_files[i], ".o") || testExt(args->input_files[i], ".s") || testExt(args->input_files[i], ".c")) && args->emit_format != EMIT_LINK) {
            addErrorf(error_context, NOPOS, WARNING, "%s: Not linking, objects and assembly will be ignored", args->input_files[i]);
        }
    }
    if ((args->library_directory_count != 0 || args->library_count != 0) && args->emit_format != EMIT_LINK) {
        addError(error_context, "Not linking, all library-specific options will be ignored", NOPOS, WARNING);
    }
    if (args->input_file_count == 0) {
        addError(error_context, "No input files specified, doing nothing", NOPOS, WARNING);
    }
    LLVMModuleRef linked_module = NULL;
    if (file_set->file_count > 1) {
        int id_length = 0;
        for (int i = 0; i < file_set->file_count; i++) {
            id_length += strlen(file_set->files[i].filename) + 1;
        }
        char* id = (char*)malloc(id_length);
        id_length = 0;
        for (int i = 0; i < file_set->file_count; i++) {
            int len = strlen(file_set->files[i].filename);
            memcpy(id + id_length, file_set->files[i].filename, len);
            id[id_length + len] = ';';
            id_length += len + 1;
        }
        id[id_length - 1] = 0;
        linked_module = LLVMModuleCreateWithName(id);
        free(id);
    }
    if (args->emit_format == EMIT_JIT && args->debug) {
        args->debug = false;
        addError(error_context, "Debug option will be ignored when using JIT", NOPOS, WARNING);
    }
    for (int i = 0; i < file_set->file_count; i++) {
        File* file = &file_set->files[i];
        if (file->ast != NULL) {
            LLVMModuleRef module = generateModuleFromAst(file->ast, file, args, error_context);
            if (linked_module != NULL) {
                if (LLVMLinkModules2(linked_module, module)) {
                    addErrorf(error_context, NOPOS, ERROR, "%s: Failed to link in the file", file->filename);
                }
            } else {
                linked_module = module;
            }
        }
    }
    for (int i = 0; i < args->input_file_count; i++) {
        if (testExt(args->input_files[i], ".ll")) {
            LLVMMemoryBufferRef buffer;
            if (LLVMCreateMemoryBufferWithContentsOfFile(args->input_files[i], &buffer, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to read file: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            LLVMModuleRef module;
            if (LLVMParseIRInContext(LLVMGetGlobalContext(), buffer, &module, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to parse ir: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            if (linked_module != NULL) {
                if (LLVMLinkModules2(linked_module, module)) {
                    addErrorf(error_context, NOPOS, ERROR, "%s: Failed to link in the file", args->input_files[i]);
                }
            } else {
                linked_module = module;
            }
        } else if (testExt(args->input_files[i], ".bc")) {
            LLVMMemoryBufferRef buffer;
            if (LLVMCreateMemoryBufferWithContentsOfFile(args->input_files[i], &buffer, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to read file: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            LLVMModuleRef module;
            if (LLVMParseBitcode(buffer, &module, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to parse bitcode: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            if (linked_module != NULL) {
                if (LLVMLinkModules2(linked_module, module)) {
                    addErrorf(error_context, NOPOS, ERROR, "%s: Failed to link in the file", args->input_files[i]);
                }
            } else {
                linked_module = module;
            }
        }
    }
    if(args->emit_format == EMIT_JIT) {
        if(args->target != NULL && !args->force_target) {
            addError(error_context, "Target options will be ignored when using JIT, force the target using --force-target", NOPOS, WARNING);
        }
        if(linked_module != NULL && getErrorCount(error_context) == 0) {
            ret = runModuleInJIT(linked_module, args, error_context);
        } else {
            ret = getErrorCount(error_context) != 0 ? 125 : 0;
        }
    } else {
        if(args->force_interpreter) {
            addError(error_context, "The --force-interpreter option is only relevant when using JIT", NOPOS, WARNING);
        }
        if(args->force_target) {
            addError(error_context, "The --force-target option is only relevant when using JIT", NOPOS, WARNING);
        }
        char* host_triple = LLVMGetDefaultTargetTriple();
        char* triple = NULL;
        if (args->target != NULL && strcmp(host_triple, args->target) != 0) {
            triple = args->target;
        } else {
            triple = host_triple;
        }
        LLVMTargetRef target = NULL;
        LLVMInitializeAllTargetInfos();
        LLVMInitializeAllTargets();
        LLVMInitializeAllAsmPrinters();
        LLVMInitializeAllTargetMCs();
        if (LLVMGetTargetFromTriple(triple, &target, &error_msg)) {
            addError(error_context, error_msg, NOPOS, ERROR);
            LLVMDisposeMessage(error_msg);
        } else {
            if (linked_module != NULL && getErrorCount(error_context) == 0) {
                LLVMCodeGenOptLevel opt_level = LLVMCodeGenLevelDefault;
                switch (max(args->size_opt, args->speed_opt)) {
                case 0:
                    opt_level = LLVMCodeGenLevelNone;
                    break;
                case 1:
                    opt_level = LLVMCodeGenLevelLess;
                    break;
                case 2:
                    opt_level = LLVMCodeGenLevelDefault;
                    break;
                case 3:
                    opt_level = LLVMCodeGenLevelAggressive;
                    break;
                }
                LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, triple, "", "", opt_level, LLVMRelocPIC, LLVMCodeModelDefault);
                LLVMTargetDataRef data_layout = LLVMCreateTargetDataLayout(target_machine);
                LLVMSetTarget(linked_module, triple);
                LLVMSetModuleDataLayout(linked_module, data_layout);
                LLVMPassManagerRef module_pass_manager = LLVMCreatePassManager();
                LLVMPassManagerBuilderRef pass_manager_builder = LLVMPassManagerBuilderCreate();
                LLVMPassManagerBuilderSetOptLevel(pass_manager_builder, args->speed_opt);
                LLVMPassManagerBuilderSetSizeLevel(pass_manager_builder, args->size_opt);
                if (args->size_opt == 0 && args->speed_opt > 1) {
                    LLVMPassManagerBuilderUseInlinerWithThreshold(pass_manager_builder, args->speed_opt * 100);
                }
                LLVMAddAnalysisPasses(target_machine, module_pass_manager);
                LLVMAddVerifierPass(module_pass_manager);
                LLVMAddCFGSimplificationPass(module_pass_manager);
                LLVMPassManagerBuilderPopulateModulePassManager(pass_manager_builder, module_pass_manager);
                if (!args->compiler_debug) {
                    LLVMRunPassManager(module_pass_manager, linked_module);
                }
                if (args->emit_format == EMIT_LLVM_IR) {
                    char* out_file = args->output_file;
                    if (out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".ll");
                    }
                    if (LLVMPrintModuleToFile(linked_module, out_file, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to output LLVM IR: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    }
                    if (args->output_file == NULL) {
                        free(out_file);
                    }
                } else if (args->emit_format == EMIT_LLVM_BC) {
                    char* out_file = args->output_file;
                    if (out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".bc");
                    }
                    if (LLVMWriteBitcodeToFile(linked_module, out_file)) {
                        addError(error_context, "Failed to output LLVM BC", NOPOS, ERROR);
                        LLVMDisposeMessage(error_msg);
                    }
                    if (args->output_file == NULL) {
                        free(out_file);
                    }
                } else if (args->emit_format == EMIT_ASM) {
                    char* out_file = args->output_file;
                    if (out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".s");
                    }
                    if (LLVMTargetMachineEmitToFile(target_machine, linked_module, out_file, LLVMAssemblyFile, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to output assembly: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    }
                    if (args->output_file == NULL) {
                        free(out_file);
                    }
                } else if (args->emit_format == EMIT_OBJ) {
                    char* out_file = args->output_file;
                    if (out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".o");
                    }
                    if (LLVMTargetMachineEmitToFile(target_machine, linked_module, out_file, LLVMObjectFile, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to output object: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    }
                    if (args->output_file == NULL) {
                        free(out_file);
                    }
                } else if (args->emit_format == EMIT_LINK) {
                    char* out_file = args->output_file;
                    if (out_file == NULL) {
                        out_file = changeExt(args->input_files[0], "");
                    }
                    int tmp_len = strlen(TMP_OBJECT_NAME_PREFIX);
                    char tmp_object_file[100] = TMP_OBJECT_NAME_PREFIX;
                    uuidv4(tmp_object_file + tmp_len);
                    tmp_object_file[tmp_len + 32] = '.';
                    tmp_object_file[tmp_len + 32 + 1] = 'o';
                    tmp_object_file[tmp_len + 32 + 2] = 0;
                    if (LLVMTargetMachineEmitToFile(target_machine, linked_module, tmp_object_file, LLVMObjectFile, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to generate object: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    } else {
                        // TODO: maybe link with different program (currently using cc)
                        ArrayList argv_to_free;
                        initArrayList(&argv_to_free);
                        ArrayList argv;
                        initArrayList(&argv);
                        pushToArrayList(&argv, (void*)"cc");
                        pushToArrayList(&argv, (void*)"-o");
                        pushToArrayList(&argv, (void*)out_file);
                        pushToArrayList(&argv, (void*)tmp_object_file);
                        for (int i = 0; i < args->input_file_count; i++) {
                            if (testExt(args->input_files[i], ".o") || testExt(args->input_files[i], ".s") || testExt(args->input_files[i], ".c")) {
                                pushToArrayList(&argv, (void*)args->input_files[i]);
                            }
                        }
                        for (int i = 0; i < args->library_directory_count; i++) {
                            int len = strlen(args->library_directories[i]);
                            char* str = (char*)malloc(len + 3);
                            str[0] = '-';
                            str[1] = 'L';
                            memcpy(str + 2, args->library_directories[i], len + 1);
                            pushToArrayList(&argv, (void*)str);
                            pushToArrayList(&argv_to_free, (void*)str);
                        }
                        for (int i = 0; i < args->library_count; i++) {
                            int len = strlen(args->libraries[i]);
                            char* str = (char*)malloc(len + 3);
                            str[0] = '-';
                            str[1] = 'l';
                            memcpy(str + 2, args->libraries[i], len + 1);
                            pushToArrayList(&argv, (void*)str);
                            pushToArrayList(&argv_to_free, (void*)str);
                        }
                        pushToArrayList(&argv, NULL);
                        char* out = (char*)malloc(MAX_LINK_ERROR);
                        if (exec("cc", (char**)argv.data, out, MAX_LINK_ERROR)) {
                            addError(error_context, out, NOPOS, ERROR);
                        }
                        free(out);
                        freeArrayList(&argv);
                        for(int i = 0; i < argv_to_free.count; i++) {
                            free(argv_to_free.data[i]);
                        }
                        freeArrayList(&argv_to_free);
                        remove(tmp_object_file);
                    }
                    if (args->output_file == NULL) {
                        free(out_file);
                    }
                }
                LLVMPassManagerBuilderDispose(pass_manager_builder);
                LLVMDisposePassManager(module_pass_manager);
                LLVMDisposeTargetData(data_layout);
                LLVMDisposeTargetMachine(target_machine);
                LLVMDisposeModule(linked_module);
            }
            LLVMDisposeMessage(host_triple);
        }
        ret = getErrorCount(error_context) != 0 ? 125 : 0;
    }
    deinitLLVM();
    return ret;
}