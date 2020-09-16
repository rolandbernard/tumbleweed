#ifndef _FILE_H_
#define _FILE_H_

#include <stddef.h>
#include <pthread.h>

#include "common/util.h"
#include "common/hashtable.h"
#include "parser/ast.h"

typedef struct File_s {
    char* filename;
    char* path;
    int offset;
    int line_count;
    int* line_offsets;
    char* data;
    int size;
    bool errors;
    Ast* ast;
} File;

typedef struct {
    const File* file;
    Position position;
    int line;
    int column;
} LineInfo;

typedef struct {
    int next_offset;
    int file_count;
    int file_capacity;
    File* files;
} FileSet;

char* compinePaths(const char* path1, const char* path2);

char* reducePath(const char* path);

void inlineReducePath(char* path);

char* getAbsolutePath(const char* path);

void initFileSet(FileSet* file_set);

void freeFileSet(FileSet* file_set);

File* addFile(FileSet* file_set, const char* filename, const char* path);

File* getFile(FileSet* file_set, const char* path);

File* getFileFromPosition(FileSet* file_set, Position position);

char* getStringCopyFromFileSet(FileSet* file_set, Position start, Position end);

char* getStringCopyFromFile(const File* file, Position start, Position end);

char* getLineCopyFromFile(const File* file, int line);

char* getStringRefFromFileSet(FileSet* file_set, Position start);

char* getStringRefFromFile(const File* file, Position start);

Position offsetToPosition(const File* file, int offset);

LineInfo offsetToLineInfo(const File* file, int offset);

LineInfo positionInFileToLineInfo(const File* file, Position position);

LineInfo positionToLineInfo(FileSet* file_set, Position position);

#endif
