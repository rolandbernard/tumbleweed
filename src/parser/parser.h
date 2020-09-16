#ifndef _PARSER_H_
#define _PARSER_H_

#include "driver/error.h"
#include "driver/file.h"
#include "parser/ast.h"

Ast* parseFile(File* file, ErrorContext* error_context);

#endif
