#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <llvm-c/Core.h>

#include "driver/file.h"
#include "parser/ast.h"
#include "common/hashtable.h"

typedef struct {
    const char* name;
    Ast* ast;
    LLVMValueRef llvm_value;
    // Only used for functions
    LLVMValueRef llvm_return;
    LLVMMetadataRef llvm_difunc;
    LLVMMetadataRef llvm_difile;
    File* file;
} Symbol;

typedef struct {
    int scope_capacity;
    int scope_count;
    HashTable* scopes;
    int current_scope;
} SymbolTable;

void initSymbolTable(SymbolTable* symbols);

void appendScope(SymbolTable* symbols);

void toNextScope(SymbolTable* symbols);

void toPrevScope(SymbolTable* symbols);

Symbol* getSymbol(SymbolTable* symbols, const char* name);

Symbol* getSymbolInCurrentScope(SymbolTable* symbols, const char* name);

void addSymbol(SymbolTable* symbols, Symbol* symbol);

void freeLowerScopes(SymbolTable* symbols);

void freeSymbolTable(SymbolTable* symbols);

#endif