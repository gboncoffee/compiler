#ifndef COMPILER_PARSER_H_
#define COMPILER_PARSER_H_

#include "compiler.h"
#include "tokenizer.h"

typedef enum {
    A,
} NodeKind;

typedef struct {
    union {
        int a;
    } this;

    NodeKind kind;
} Node;

typedef struct Ast {
    Node this;
    struct Ast* next;
} Ast;

Ast* Parse(Arena* arena, Tokens* tokens, Errors** errors);

#endif // COMPILER_PARSER_H_
