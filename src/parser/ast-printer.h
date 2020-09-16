#ifndef _AST_PRINTER_H_
#define _AST_PRINTER_H_

#include <stdio.h>

#include "parser/ast.h"

void printAst(FILE* file, Ast* ast, int indent);

#endif