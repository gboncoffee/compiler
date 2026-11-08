#include "codegen.h"

void Codegen(Arena* arena, Ast* ast, FILE* output)
{
    (void)arena;
    (void)output;
    printf("Program: %.*s\n", (int)ast->name.size, ast->name.ptr);
}
