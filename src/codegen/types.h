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

LLVMMetadataRef generateTypeMeta(LLVMDIBuilderRef dibuilder, LLVMTypeRef type, LLVMMetadataRef file);

#endif