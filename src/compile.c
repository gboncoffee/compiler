#include "arena.h"
#include "codegen.h"
#include "compiler.h"
#include "parser.h"
#include "semantics.h"
#include "tokenizer.h"

#include <assert.h>
#include <stdio.h>

void PrintError(Error error, const char* file)
{
    printf(
        "%s:%d:%d: %s: %s.\n",
        file,
        (int) error.line,
        (int) error.column,
        error.message,
        error.dynamic
    );
}

Errors* Compile(Arena* arena, char* program, FILE* output)
{
    Errors* errors = nullptr;
    auto    tokens = Tokenize(arena, program, &errors);

    auto tree = Parse(arena, tokens, &errors);
    if (errors != nullptr) {
        return errors;
    }

    errors = SemanticAnalysis(arena, tree);
    if (errors != nullptr) {
        return errors;
    }

    Codegen(arena, tree, output);

    return nullptr;
}
