
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "driver/file.h"

char* reducePath(const char* path) {
    int len = (int)strlen(path);
    char* ret = (char*)malloc(len + 1);
    int path_pos = 0;
    int ret_pos = 0;
    int reducable = 0;
    if (path[0] == '/') {
        ret[0] = '/';
        path_pos = 1;
        ret_pos = 1;
    }
    while (path_pos < len) {
        while (path[path_pos] == '/') {
            path_pos++;
        }
        int next_slash = path_pos;
        while (next_slash < len && path[next_slash] != '/') {
            next_slash++;
        }
        if (next_slash == path_pos + 1 && path[path_pos] == '.') {
            path_pos = next_slash + 1;
        } else if (next_slash == path_pos + 2 && path[path_pos] == '.' && path[path_pos + 1] == '.') {
            if (reducable > 0) {
                ret_pos -= 1;
                while (ret_pos > 0 && ret[ret_pos - 1] != '/') {
                    ret_pos--;
                }
                path_pos = next_slash + 1;
                reducable--;
            } else {
                while (path_pos <= next_slash && path_pos < len) {
                    ret[ret_pos] = path[path_pos];
                    ret_pos++;
                    path_pos++;
                }
            }
        } else {
            while (path_pos <= next_slash && path_pos < len) {
                ret[ret_pos] = path[path_pos];
                ret_pos++;
                path_pos++;
            }
            reducable++;
        }
    }
    if (ret_pos > 1 && ret[ret_pos - 1] == '/') {
        ret[ret_pos - 1] = 0;
    } else {
        ret[ret_pos] = 0;
    }
    return ret;
}

void inlineReducePath(char* path) {
    int len = (int)strlen(path);
    int path_pos = 0;
    int ret_pos = 0;
    int reducable = 0;
    if (path[0] == '/') {
        path[0] = '/';
        path_pos = 1;
        ret_pos = 1;
    }
    while (path_pos < len) {
        while (path[path_pos] == '/') {
            path_pos++;
        }
        int next_slash = path_pos;
        while (next_slash < len && path[next_slash] != '/') {
            next_slash++;
        }
        if (next_slash == path_pos + 1 && path[path_pos] == '.') {
            path_pos = next_slash + 1;
        } else if (next_slash == path_pos + 2 && path[path_pos] == '.' && path[path_pos + 1] == '.') {
            if (reducable > 0) {
                ret_pos -= 1;
                while (ret_pos > 0 && path[ret_pos - 1] != '/') {
                    ret_pos--;
                }
                path_pos = next_slash + 1;
                reducable--;
            } else {
                while (path_pos <= next_slash && path_pos < len) {
                    path[ret_pos] = path[path_pos];
                    ret_pos++;
                    path_pos++;
                }
            }
        } else {
            while (path_pos <= next_slash && path_pos < len) {
                path[ret_pos] = path[path_pos];
                ret_pos++;
                path_pos++;
            }
            reducable++;
        }
    }
    if (ret_pos > 1 && path[ret_pos - 1] == '/') {
        path[ret_pos - 1] = 0;
    } else {
        path[ret_pos] = 0;
    }
}

char* compinePaths(const char* path1, const char* path2) {
    int len1 = (int)strlen(path1);
    int len2 = (int)strlen(path2);
    char* ret = (char*)malloc(len1 + len2 + 2);
    memcpy(ret, path1, len1);
    ret[len1] = '/';
    memcpy(ret + len1 + 1, path2, len2);
    ret[len1 + len2 + 1] = 0;
    inlineReducePath(ret);
    return ret;
}

char* getAbsolutePath(const char* path) {
    if (path[0] == '/') {
        return reducePath(path);
    } else {
        char* cwd = getcwd(NULL, 0);
        char* ret = compinePaths(cwd, path);
        free(cwd);
        return ret;
    }
}

void initFileSet(FileSet* file_set) {
    file_set->next_offset = 100;
    file_set->file_count = 0;
    file_set->file_capacity = 0;
    file_set->files = NULL;
}

void freeFileSet(FileSet* file_set) {
    for (int i = 0; i < file_set->file_count; i++) {
        free(file_set->files[i].data);
        free(file_set->files[i].filename);
        free(file_set->files[i].path);
        free(file_set->files[i].line_offsets);
        freeAst(file_set->files[i].ast);
    }
    free(file_set->files);
    initFileSet(file_set);
}

File* addFile(FileSet* file_set, const char* name, const char* path) {
    char* absolute_path = getAbsolutePath(path);
    char* filename = reducePath(name);
    for (int i = 0; i < file_set->file_count; i++) {
        if (strcmp(file_set->files[i].path, absolute_path) == 0) {
            free(filename);
            free(absolute_path);
            File* ret = &(file_set->files[i]);
            return ret;
        }
    }
    FILE* fileptr = fopen(absolute_path, "r");
    if (fileptr == NULL) {
        free(filename);
        free(absolute_path);
        return NULL;
    } else {
        int size = ftell(fileptr);
        fseek(fileptr, 0, SEEK_END);
        size = ftell(fileptr) - size;
        fseek(fileptr, 0, SEEK_SET);
        if (file_set->file_capacity == file_set->file_count) {
            file_set->file_capacity = file_set->file_capacity == 0 ? 4 : 2 * file_set->file_capacity;
            file_set->files = (File*)realloc(file_set->files, sizeof(File) * file_set->file_capacity);
        }
        file_set->file_count++;
        File* file = &(file_set->files[file_set->file_count - 1]);
        file->filename = filename;
        file->path = absolute_path;
        file->offset = file_set->next_offset;
        file_set->next_offset += size;
        file->size = size;
        file->data = (char*)malloc(size + 1);
        fread(file->data, 1, size, fileptr);
        file->data[size] = 0;
        file->line_count = 1;
        for (int i = 0; i < size; i++) {
            if (file->data[i] == '\n') {
                file->line_count++;
            }
        }
        file->line_offsets = (int*)malloc(sizeof(int) * file->line_count);
        file->line_offsets[0] = 0;
        for (int i = 0, j = 1; i < size; i++) {
            if (file->data[i] == '\n') {
                file->line_offsets[j] = i + 1;
                j++;
            }
        }
        fclose(fileptr);
        file->ast = NULL;
        return file;
    }
}

File* getFile(FileSet* file_set, const char* filename) {
    char* path = getAbsolutePath(filename);
    for (int i = 0; i < file_set->file_count; i++) {
        if (strcmp(file_set->files[i].path, path) == 0) {
            free(path);
            File* ret = &(file_set->files[i]);
            return ret;
        }
    }
    free(path);
    return NULL;
}

File* getFileFromPosition(FileSet* file_set, Position position) {
    int i = 0, j = file_set->file_count;
    while (i + 1 != j) {
        int h = (i + j) / 2;
        if (file_set->files[h].offset > position) {
            j = h;
        } else {
            i = h;
        }
    }
    File* ret = &(file_set->files[i]);
    return ret;
}

char* getStringCopyFromFile(const File* file, Position start, Position end) {
    char* ret = (char*)malloc(end - start + 1);
    memcpy(ret, file->data + start - file->offset, end - start);
    ret[end - start] = 0;
    return ret;
}

char* getLineCopyFromFile(const File* file, int line) {
    if(line <= file->line_count && line >= 1) {
        int start = file->line_offsets[line - 1];
        int end = file->line_offsets[line];
        char* ret = (char*)malloc(end - start + 1);
        memcpy(ret, file->data + start, end - start);
        ret[end - start] = 0;
        return ret;
    } else {
        return NULL;
    }
}

char* getStringCopyFromFileSet(FileSet* file_set, Position start, Position end) { return getStringCopyFromFile(getFileFromPosition(file_set, start), start, end); }

char* getStringRefFromFile(const File* file, Position start) { return file->data + start - file->offset; }

char* getStringRefFromFileSet(FileSet* file_set, Position start) { return getStringRefFromFile(getFileFromPosition(file_set, start), start); }

Position offsetToPosition(const File* file, int offset) { return file->offset + offset; }

LineInfo offsetToLineInfo(const File* file, int offset) {
    int i = 0, j = file->line_count;
    while (i + 1 != j) {
        int h = (i + j) / 2;
        if (file->line_offsets[h] > offset) {
            j = h;
        } else {
            i = h;
        }
    }
    LineInfo line_info;
    line_info.file = file;
    line_info.position = offsetToPosition(file, offset);
    line_info.line = j;
    line_info.column = (offset - file->line_offsets[i]) + 1;
    return line_info;
}

LineInfo positionInFileToLineInfo(const File* file, Position position) { return offsetToLineInfo(file, position - file->offset); }

LineInfo positionToLineInfo(FileSet* file_set, Position position) { return positionInFileToLineInfo(getFileFromPosition(file_set, position), position); }
