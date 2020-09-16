#ifndef _COMPILE_H_
#define _COMPILE_H_

#include "driver/args.h"
#include "driver/error.h"
#include "driver/file.h"

void compile(Args* args, ErrorContext* error_context, FileSet* file_set);

#endif