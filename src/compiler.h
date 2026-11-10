#ifndef COMPILER_COMPILER_H_
#define COMPILER_COMPILER_H_

#include "arena.h"
#include "string.h"

#include <stdio.h>

typedef struct {
    unsigned long line;
    unsigned long column;
} Location;

typedef struct {
    Location location;
    String   message;
    String   dynamic;
} Error;

constexpr Error ZERO_ERROR = {
    {0, 0},
    STR(""),
    STR("")
};

bool ErrorOk(Error error);
void PrintError(Error error, const char* file);

Error Compile(Arena* arena, char* program, FILE* output);

#endif // COMPILER_COMPILER_H_
