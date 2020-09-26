
#include <fcntl.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/OrcBindings.h>
#include <llvm-c/Support.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "driver/jit.h"

#define MAX_JIT_ERROR 10000

typedef struct {
    LLVMOrcJITStackRef jit_stack;
    Args* args;
    int error_pipe;
} ResolverContext;

void fallbackFunction() { exit(125); }

static void fdwrite(int fd, const char* str) { write(fd, str, strlen(str)); }

LLVMOrcTargetAddress symbolResolver(const char* name, void* ctx) {
    ResolverContext* context = (ResolverContext*)ctx;
    void* address = LLVMSearchForAddressOfSymbol(name);
    if (address == NULL) {
        fdwrite(context->error_pipe, "Unresolved symbol '");
        fdwrite(context->error_pipe, name);
        fdwrite(context->error_pipe, "'\n");
        address = (void*)fallbackFunction;
    }
    return (LLVMOrcTargetAddress)address;
}

typedef int (*MainFunction)(int argc, const char* const* argv, const char* const* env);

// The JIT will run in a child process
int runModuleInJIT(LLVMModuleRef module, Args* args, ErrorContext* error_context) {
    int ret = 125;
    char* triple;
    if(args->target != NULL && args->force_target) {
        LLVMInitializeAllTargetInfos();
        LLVMInitializeAllTargets();
        LLVMInitializeAllAsmPrinters();
        LLVMInitializeAllTargetMCs();
        triple = args->target;
    } else {
        LLVMInitializeNativeTarget();
        LLVMInitializeNativeAsmPrinter();
        triple = LLVMGetDefaultTargetTriple();
    }
    LLVMTargetRef target;
    char* error_msg;
    if (LLVMGetTargetFromTriple(triple, &target, &error_msg)) {
        addError(error_context, error_msg, NOPOS, ERROR);
        LLVMDisposeMessage(error_msg);
    } else {
        LLVMLoadLibraryPermanently(NULL);
        if (!args->force_interpreter && LLVMTargetHasJIT(target)) {
            int err_pipe[2];
            pipe(err_pipe);
            fcntl(err_pipe[0], F_SETFL, fcntl(err_pipe[0], F_GETFL) | O_NONBLOCK);
            int cid = fork();
            if (cid == 0) {
                int ret = 0;
                char* host_cpu = LLVMGetHostCPUName();
                char* host_feartures = LLVMGetHostCPUFeatures();
                LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, triple, host_cpu, host_feartures, LLVMCodeGenLevelNone, LLVMRelocPIC, LLVMCodeModelJITDefault);
                LLVMOrcJITStackRef jit_stack = LLVMOrcCreateInstance(machine);
                ResolverContext context = {
                    .jit_stack = jit_stack,
                    .args = args,
                    .error_pipe = err_pipe[1],
                };
                LLVMOrcModuleHandle module_handle;
                LLVMErrorRef error = LLVMOrcAddLazilyCompiledIR(jit_stack, &module_handle, module, symbolResolver, (void*)&context);
                if (error) {
                    char* error_msg = LLVMGetErrorMessage(error);
                    addError(error_context, error_msg, NOPOS, ERROR);
                    LLVMDisposeErrorMessage(error_msg);
                } else {
                    LLVMOrcTargetAddress main_address = 0;
                    error = LLVMOrcGetSymbolAddress(jit_stack, &main_address, "main");
                    if (error || main_address == 0) {
                        char* error_msg = LLVMGetErrorMessage(error);
                        fdwrite(err_pipe[1], "Can't find the main function: ");
                        fdwrite(err_pipe[1], error_msg);
                        fdwrite(err_pipe[1], "\n");
                        LLVMDisposeErrorMessage(error_msg);
                    } else {
                        ret = ((MainFunction)main_address)(args->argc, args->argv, args->env);
                    }
                }
                LLVMOrcDisposeInstance(jit_stack);
                LLVMDisposeTargetMachine(machine);
                LLVMDisposeMessage(host_cpu);
                LLVMDisposeMessage(host_feartures);
                LLVMShutdown();
                close(err_pipe[1]);
                exit(ret);
            } else if (cid > 0) {
                int status;
                waitpid(cid, &status, 0);
                char* out = (char*)malloc(MAX_JIT_ERROR);
                int len = read(err_pipe[0], out, MAX_JIT_ERROR);
                if (len != -1) {
                    int last_start = 0;
                    for (int i = 0; i < len; i++) {
                        if (out[i] == '\n') {
                            out[i] = 0;
                            addError(error_context, out + last_start, NOPOS, ERROR);
                            last_start = i + 1;
                        }
                    }
                }
                close(err_pipe[0]);
                free(out);
                ret = status;
            } else {
                addError(error_context, "Failed to fork", NOPOS, ERROR);
                close(err_pipe[0]);
                close(err_pipe[1]);
                ret = getErrorCount(error_context) != 0 ? 125 : 0;
            }
        } else {
            LLVMLinkInInterpreter();
            LLVMExecutionEngineRef engine = NULL;
            if (LLVMCreateInterpreterForModule(&engine, module, &error_msg)) {
                addError(error_context, error_msg, NOPOS, ERROR);
                LLVMDisposeMessage(error_msg);
            } else {
                LLVMValueRef main = LLVMGetNamedFunction(module, "main");
                if(main == NULL) {
                    addError(error_context, "Can't find the main function", NOPOS, ERROR);
                } else {
                    ret = LLVMRunFunctionAsMain(engine, main, args->argc, args->argv, args->env);
                }
            }
            LLVMDisposeExecutionEngine(engine);
        }
    }
    if(args->target == NULL || !args->force_target) {
        LLVMDisposeMessage(triple);
    }
    return ret;
}
