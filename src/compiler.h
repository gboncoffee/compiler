#ifndef COMPILER_COMPILER_H_
#define COMPILER_COMPILER_H_

#include "arena.h"

#include <stdio.h>

typedef struct {
    unsigned long line;
    unsigned long column;
    const char*   message;
    char*         dynamic;
} Error;

typedef struct Errors {
    Error this;
    struct Errors* next;
} Errors;

void PrintError(Error error, const char* file);

Errors* Compile(Arena* arena, char* program, FILE* output);

#endif // COMPILER_COMPILER_H_
