
#include <stdlib.h>

#include "codegen/symbols.h"

void initSymbolTable(SymbolTable* symbols) {
    symbols->scope_capacity = 0;
    symbols->scope_count = 0;
    symbols->scopes = NULL;
    symbols->current_scope = -1;
}

void freeLowerScopes(SymbolTable* symbols) {
    for(int i = symbols->current_scope + 1; i < symbols->scope_count; i++) {
        for(int j = 0; j < symbols->scopes[i].capacity; j++) {
            if(symbols->scopes[i].data[j].key != NULL && symbols->scopes[i].data[j].key != DELETED) {
                free(symbols->scopes[i].data[j].value);
            }
        }
        freeHashTable(&symbols->scopes[i]);
    }
    symbols->scope_count = symbols->current_scope + 1;
}

void freeSymbolTable(SymbolTable* symbols) {
    symbols->current_scope = -1;
    freeLowerScopes(symbols);
    free(symbols->scopes);
    initSymbolTable(symbols);
}

void appendScope(SymbolTable* symbols) {
    if(symbols->scope_capacity == symbols->scope_count) {
        symbols->scope_capacity = symbols->scope_capacity == 0 ? 4 : 2 * symbols->scope_capacity;
        symbols->scopes = (HashTable*)realloc(symbols->scopes, symbols->scope_capacity * sizeof(HashTable*));
    }
    initHashTable(&symbols->scopes[symbols->scope_count]);
    symbols->scope_count++;
}

void toNextScope(SymbolTable* symbols) {
    if(symbols->scope_count > symbols->current_scope) {
        symbols->current_scope++;
    }
}

void toPrevScope(SymbolTable* symbols) {
    if(symbols->current_scope > -1) {
        symbols->current_scope--;
    }
}

Symbol* getSymbol(SymbolTable* symbols, const char* name) {
    for(int i = symbols->current_scope; i >= 0; i--) {
        Symbol* found = (Symbol*)getFromHashTable(&symbols->scopes[i], name);
        if(found != NULL) {
            return found;
        }
    }
    return NULL;
}

Symbol* getSymbolInCurrentScope(SymbolTable* symbols, const char* name) {
    if(symbols->current_scope >= 0) {
        return (Symbol*)getFromHashTable(&symbols->scopes[symbols->current_scope], name);
    } else {
        return NULL;
    }
}

void addSymbol(SymbolTable* symbols, Symbol* symbol) {
    if(symbols->current_scope >= 0) {
        setToHashTable(&symbols->scopes[symbols->current_scope], symbol->name, symbol);
    }
}
