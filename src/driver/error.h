#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>

#include "driver/file.h"

typedef enum {
    ERROR,
    WARNING,
    NOTE,
} ErrorLevel;

typedef struct {
    char* message;
    Position position;
    ErrorLevel level;
} Error;

typedef struct {
    int error_count;
    int error_capacity;
    Error* errors;
} ErrorContext;

void initErrorContext(ErrorContext* context);

void freeErrorContext(ErrorContext* context);

void addError(ErrorContext* context, const char* message, Position position, ErrorLevel level);

void addErrorf(ErrorContext* context, Position position, ErrorLevel level, const char* messag, ...);

void printErrors(FILE* file, ErrorContext* context, FileSet* file_set);

int getErrorCount(ErrorContext* context);

#endif
