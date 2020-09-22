
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

void compile(Args* args, ErrorContext* error_context, FileSet* file_set) {
    initLLVM();
    char* error_msg;
    for (int i = 0; i < args->input_file_count; i++) {
        if(testExt(args->input_files[i], ".tumble")) {
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
        } else if((testExt(args->input_files[i], ".o") || testExt(args->input_files[i], ".s")) && args->emit_format != EMIT_LINK) {
            addErrorf(error_context, NOPOS, WARNING, "%s: Not linking, objects and assembly will be ignored", args->input_files[i]);
        }
    }
    if((args->library_directory_count != 0 || args->library_count != 0) && args->emit_format != EMIT_LINK) {
        addError(error_context, "Not linking, all library-specific options will be ignored", NOPOS, WARNING);
    }
    LLVMModuleRef linked_module;
    if(file_set->file_count > 1) {
        int id_length = 0;
        for(int i = 0; i < file_set->file_count; i++) {
            id_length += strlen(file_set->files[i].filename) + 1;
        }
        char* id = (char*)malloc(id_length);
        id_length = 0;
        for(int i = 0; i < file_set->file_count; i++) {
            int len = strlen(file_set->files[i].filename);
            memcpy(id + id_length, file_set->files[i].filename, len);
            id[id_length + len] = ';';
            id_length += len + 1;
        }
        id[id_length-1] = 0;
        linked_module = LLVMModuleCreateWithName(id);
        free(id);
    }
    for (int i = 0; i < file_set->file_count; i++) {
        File* file = &file_set->files[i];
        if(file->ast != NULL) {
            LLVMModuleRef module = generateModuleFromAst(file->ast, file, args, error_context);
            if(file_set->file_count > 1) {
                if(LLVMLinkModules2(linked_module, module)) {
                    addErrorf(error_context, NOPOS, ERROR, "%s: Failed to link in the file", file->filename);
                }
            } else {
                linked_module = module;
            }
        }
    }
    for (int i = 0; i < args->input_file_count; i++) {
        if(testExt(args->input_files[i], ".ll")) {
            LLVMMemoryBufferRef buffer;
            if(LLVMCreateMemoryBufferWithContentsOfFile(args->input_files[i], &buffer, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to read file: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            LLVMModuleRef module;
            if(LLVMParseIRInContext(LLVMGetGlobalContext(), buffer, &module, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to parse bitcode", args->input_files[i]);
                LLVMDisposeMessage(error_msg);
            }
            if(LLVMLinkModules2(linked_module, module)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to link in the file", args->input_files[i]);
            }
        } else if(testExt(args->input_files[i], ".bc")) {
            LLVMMemoryBufferRef buffer;
            if(LLVMCreateMemoryBufferWithContentsOfFile(args->input_files[i], &buffer, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to read file: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            LLVMModuleRef module;
            if(LLVMParseBitcode(buffer, &module, &error_msg)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to parse bitcode: %s", args->input_files[i], error_msg);
                LLVMDisposeMessage(error_msg);
            }
            if(LLVMLinkModules2(linked_module, module)) {
                addErrorf(error_context, NOPOS, ERROR, "%s: Failed to link in the file", args->input_files[i]);
            }
        }
    }
    if(linked_module != NULL) {
        char* host_triple = LLVMGetDefaultTargetTriple();
        char* triple = NULL;
        if(args->target != NULL && strcmp(host_triple, args->target) != 0) {
            triple = args->target;
        } else {
            triple = host_triple;
        }
        LLVMTargetRef target = NULL;
        LLVMInitializeAllTargetInfos();
        LLVMInitializeAllTargets();
        LLVMInitializeAllAsmPrinters();
        LLVMInitializeAllTargetMCs();
        if(LLVMGetTargetFromTriple(triple, &target, &error_msg)) {
            addError(error_context, error_msg, NOPOS, ERROR);
            LLVMDisposeMessage(error_msg);
        } else {
            LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, triple, "", "", LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);
            LLVMTargetDataRef data_layout = LLVMCreateTargetDataLayout(target_machine);
            LLVMPassManagerRef module_pass_manager = LLVMCreatePassManager();
            LLVMPassManagerBuilderRef pass_manager_builder = LLVMPassManagerBuilderCreate();
            LLVMPassManagerBuilderSetOptLevel(pass_manager_builder, args->speed_opt);
            LLVMPassManagerBuilderSetSizeLevel(pass_manager_builder, args->size_opt);
            LLVMAddAnalysisPasses(target_machine, module_pass_manager);
            
            if(getErrorCount(error_context) == 0) {
                LLVMAddVerifierPass(module_pass_manager);
                LLVMAddCFGSimplificationPass(module_pass_manager);
                LLVMSetTarget(linked_module, triple);
                LLVMSetModuleDataLayout(linked_module, data_layout);
                LLVMPassManagerBuilderPopulateModulePassManager(pass_manager_builder, module_pass_manager);
                LLVMRunPassManager(module_pass_manager, linked_module);
            
                if(args->emit_format == EMIT_LLVM_IR) {
                    char* out_file = args->output_file;
                    if(out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".ll");
                    }
                    if(LLVMPrintModuleToFile(linked_module, out_file, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to output LLVM IR: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    }
                    if(args->output_file == NULL) {
                        free(out_file);
                    }
                } else if(args->emit_format == EMIT_LLVM_BC) {
                    char* out_file = args->output_file;
                    if(out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".bc");
                    }
                    if(LLVMWriteBitcodeToFile(linked_module, out_file)) {
                        addError(error_context, "Failed to output LLVM BC", NOPOS, ERROR);
                        LLVMDisposeMessage(error_msg);
                    }
                    if(args->output_file == NULL) {
                        free(out_file);
                    }
                } else if(args->emit_format == EMIT_ASM) {
                    char* out_file = args->output_file;
                    if(out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".s");
                    }
                    if(LLVMTargetMachineEmitToFile(target_machine, linked_module, out_file, LLVMAssemblyFile, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to output assembly: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    }
                    if(args->output_file == NULL) {
                        free(out_file);
                    }
                } else if(args->emit_format == EMIT_OBJ) {
                    char* out_file = args->output_file;
                    if(out_file == NULL) {
                        out_file = changeExt(args->input_files[0], ".o");
                    }
                    if(LLVMTargetMachineEmitToFile(target_machine, linked_module, out_file, LLVMObjectFile, &error_msg)) {
                        addErrorf(error_context, NOPOS, ERROR, "Failed to output object: %s", error_msg);
                        LLVMDisposeMessage(error_msg);
                    }
                    if(args->output_file == NULL) {
                        free(out_file);
                    }
                } else if(args->emit_format == EMIT_LINK) {
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
                        int obj_asm_count = 0;
                        for (int i = 0; i < args->input_file_count; i++) {
                            if(testExt(args->input_files[i], ".o") || testExt(args->input_files[i], ".s")) {
                                obj_asm_count++;
                            }
                        }
                        int argc = 4 + obj_asm_count + args->library_count + args->library_directory_count;
                        char** argv = (char**)malloc(sizeof(char*) * (argc + 1));
                        argv[0] = "cc";
                        argv[1] = "-o";
                        argv[2] = out_file;
                        argv[3] = tmp_object_file;
                        for (int i = 0; i < args->input_file_count; i++) {
                            if(testExt(args->input_files[i], ".o") || testExt(args->input_files[i], ".s")) {
                                argv[4 + i] = args->input_files[i];
                            }
                        }
                        for (int i = 0; i < args->library_directory_count; i++) {
                            int len = strlen(args->library_directories[i]);
                            argv[4 + obj_asm_count + i] = (char*)malloc(len + 3);
                            argv[4 + obj_asm_count + i][0] = '-';
                            argv[4 + obj_asm_count + i][1] = 'L';
                            memcpy(argv[4 + obj_asm_count + i] + 2, args->library_directories[i], len + 1);
                        }
                        for (int i = 0; i < args->library_count; i++) {
                            int len = strlen(args->libraries[i]);
                            argv[4 + obj_asm_count + args->library_directory_count + i] = (char*)malloc(len + 3);
                            argv[4 + obj_asm_count + args->library_directory_count + i][0] = '-';
                            argv[4 + obj_asm_count + args->library_directory_count + i][1] = 'l';
                            memcpy(argv[4 + obj_asm_count + args->library_directory_count + i] + 2, args->libraries[i], len + 1);
                        }
                        argv[argc] = NULL;
                        char* out = (char*)malloc(MAX_LINK_ERROR);
                        if(exec("cc", argv, out, MAX_LINK_ERROR)) {
                            addError(error_context, out, NOPOS, ERROR);
                        }
                        free(out);
                        for (int i = 0; i < args->library_directory_count; i++) {
                            free(argv[4 + obj_asm_count + i]);
                        }
                        for (int i = 0; i < args->library_count; i++) {
                            free(argv[4 + obj_asm_count + args->library_directory_count + i]);
                        }
                        free(argv);
                        remove(tmp_object_file);
                    }
                    if (args->output_file == NULL) {
                        free(out_file);
                    }
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
    deinitLLVM();
}