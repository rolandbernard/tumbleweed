#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"
#include "codegen/symbols.h"

bool generateRoot(AstRoot* ast, File* file, LLVMMetadataRef file_meta, LLVMModuleRef module, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueVariable(AstVariableDefinition* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueParameter(AstParameterDefinition* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIfElse(AstIfElse* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueForLoop(AstForLoop* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueReturn(AstUnaryOperation* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueCodeBlock(AstCodeBlock* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIntegerLiteral(AstIntegerLiteral* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueFloatLiteral(AstFloatLiteral* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueStringLiteral(AstStringLiteral* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueCharacterLiteral(AstCharacterLiteral* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueVariableAccess(AstVariableAccess* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueCall(AstCall* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueIndex(AstIndex* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

LLVMValueRef generateValueInFunction(Ast* ast, Symbol* function, LLVMDIBuilderRef dibuilder, LLVMBuilderRef builder, Args* args, SymbolTable* symbols, ErrorContext* error_context);

#endif