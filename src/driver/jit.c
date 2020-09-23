
#include <llvm-c/OrcBindings.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Support.h>

#include <stdio.h>

#include "driver/jit.h"

typedef struct {
    LLVMOrcJITStackRef jit_stack;
    Args* args;
    ErrorContext* error_context;
} ResolverContext;

void fallbackFunction() { }

LLVMOrcTargetAddress symbolResolver(const char* name, void* ctx) {
    ResolverContext* context = (ResolverContext*)ctx;
    void* address = LLVMSearchForAddressOfSymbol(name);
    fprintf(stderr, "symbol: %s, address: %p", name, address);
    if(address == NULL) {
        addErrorf(context->error_context, NOPOS, ERROR, "Unresolved symbol '%s'", name);
        address = (void*)fallbackFunction;
    }
    return (LLVMOrcTargetAddress)address;
}

typedef void (*MainFunction)();

void runModuleInJIT(LLVMModuleRef module, Args* args, ErrorContext* error_context) {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMLinkInMCJIT();
    char* triple = LLVMGetDefaultTargetTriple();
    LLVMTargetRef target;
    char* error_msg;
    if(LLVMGetTargetFromTriple(triple, &target, &error_msg)) {
        addError(error_context, error_msg, NOPOS, ERROR);
        LLVMDisposeMessage(error_msg);
    } else {
        char* host_cpu = LLVMGetHostCPUName();
        char* host_feartures = LLVMGetHostCPUFeatures();
        LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, triple, host_cpu, host_feartures, LLVMCodeGenLevelNone, LLVMRelocPIC, LLVMCodeModelJITDefault);
        LLVMOrcJITStackRef jit_stack = LLVMOrcCreateInstance(machine);
        ResolverContext context = {
            .jit_stack = jit_stack,
            .args = args,
            .error_context = error_context,
        };
        LLVMOrcModuleHandle module_handle;
        LLVMErrorRef error = LLVMOrcAddLazilyCompiledIR(jit_stack, &module_handle, module, symbolResolver, (void*)&context);
        if(error) {
            char* error_msg = LLVMGetErrorMessage(error);
            addError(error_context, error_msg, NOPOS, ERROR);
            LLVMDisposeErrorMessage(error_msg);
        } else {
            LLVMOrcTargetAddress main_address = 0;
            error = LLVMOrcGetSymbolAddress(jit_stack, &main_address, "main");
            if (error) {
                char* error_msg = LLVMGetErrorMessage(error);
                addErrorf(error_context, NOPOS, ERROR, "Can't find the main function: %s", error_msg);
                LLVMDisposeErrorMessage(error_msg);
            } else {
                ((MainFunction)main_address)();
            }
        }
        LLVMOrcDisposeInstance(jit_stack);
        LLVMDisposeTargetMachine(machine);
        LLVMDisposeMessage(host_cpu);
        LLVMDisposeMessage(host_feartures);
    }
    LLVMDisposeMessage(triple);
}
