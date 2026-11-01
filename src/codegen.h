#ifndef COMPILER_CODEGEN_H_
#define COMPILER_CODEGEN_H_

#include "arena.h"
#include "parser.h"

#include <stdio.h>

void Codegen(Arena* arena, Ast* ast, FILE* output);

#endif // COMPILER_CODEGEN_H_
