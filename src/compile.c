#include "arena.h"
#include "codegen.h"
#include "compiler.h"
#include "parser.h"
#include "semantics.h"
#include "tokenizer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

bool ErrorOk(Error error)
{
    return error.location.column == 0 && error.location.line == 0 &&
        error.message == nullptr && error.dynamic == nullptr;
}

void PrintError(Error error, const char* file)
{
    if (error.dynamic != nullptr) {
        printf(
            "%s:%d:%d: %s: %s.\n",
            file,
            (int) error.location.line,
            (int) error.location.column,
            error.message,
            error.dynamic
        );
        return;
    }
    printf(
        "%s:%d:%d: %s.\n",
        file,
        (int) error.location.line,
        (int) error.location.column,
        error.message
    );
}

Error Compile(Arena* arena, char* program, FILE* output)
{
    auto error = ZERO_ERROR;

    auto tokens = Tokenize(arena, program, &error);
    if (!ErrorOk(error))
        return error;

    auto tree = Parse(arena, program, tokens, &error);
    if (!ErrorOk(error))
        return error;

    error = SemanticAnalysis(arena, tree);
    if (!ErrorOk(error))
        return error;

    Codegen(arena, tree, output);

    return error;
}
