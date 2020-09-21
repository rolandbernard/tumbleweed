#ifndef _TYPES_H_
#define _TYPES_H_

#include <llvm-c/Core.h>

#include "parser/ast.h"
#include "driver/args.h"
#include "driver/error.h"

LLVMTypeRef generateTypeBase(AstVariableAccess* ast, Args* args, ErrorContext* error_context);

LLVMTypeRef generateTypeReference(AstUnaryOperation* ast, Args* args, ErrorContext* error_context);

LLVMTypeRef generateTypeAddress(Ast* ast, Args* args, ErrorContext* error_context);

LLVMTypeRef generateType(Ast* ast, Args* args, ErrorContext* error_context);

LLVMMetadataRef generateTypeMetaBase(AstVariableAccess* ast, LLVMDIBuilderRef dibuilder, Args* args);

LLVMMetadataRef generateTypeMetaReference(AstUnaryOperation* ast, LLVMDIBuilderRef dibuilder, Args* args);

LLVMMetadataRef generateTypeMetaAddress(Ast* ast, LLVMDIBuilderRef dibuilder, Args* args);

LLVMMetadataRef generateTypeMeta(Ast* ast, LLVMDIBuilderRef dibuilder, Args* args);

#endif