#ifndef COMPILER_PARSER_H_
#define COMPILER_PARSER_H_

#include "compiler.h"
#include "string.h"
#include "tokenizer.h"

typedef enum {
    TypeInteger,
    TypeReal,
    TypeArray,
} TypeKind;

struct Type;

typedef struct {
    struct Type*  content;
    unsigned long begin;
    unsigned long end;
} Array;

typedef struct {
    Array    array;
    TypeKind kind;
} Type;

struct Expression;

typedef struct {
    String             operator;
    struct Expression* left;
    struct Expression* right;
} Operation;

typedef struct {
    Type   type;
    String content;
} Literal;

typedef struct Compound {
    struct Expression* this;
    struct Compound* next;
} Compound;

typedef struct {
    struct Expression* condition;
    struct Expression* body;
    struct Expression* elsee;
} If;

typedef struct {
    struct Expression* condition;
    struct Expression* body;
} While;

typedef enum {
    ExpressionLiteral,
    ExpressionName,
    ExpressionOperation,
    ExpressionIf,
    ExpressionWhile,
    ExpressionCompound,
} ExpressionKind;

typedef struct {
    union {
        Literal   literal;
        String    name;
        Operation operation;
        If        iff;
        While     whilee;
        Compound  compound;
    } this;

    ExpressionKind kind;
} Expression;

typedef struct Names {
    String this;
    struct Names* next;
} Names;

typedef struct {
    Names* names;
    Type   type;
} Declaration;

typedef struct Declarations {
    Declaration this;
    struct Declarations* next;
} Declarations;

typedef struct {
    String name;
    Type   type;
    bool   reference;
} Parameter;

typedef struct {
    Parameter this;
    struct Parameters* next;
} Parameters;

typedef struct {
    Parameters parameters;
    Expression body;
} Subprogram;

typedef struct Subprograms {
    Subprogram* this;
    struct Subprograms* next;
} Subprograms;

typedef struct Ast {
    String        name;
    Declarations* declarations;
    Subprograms*  subprograms;
    Expression    main;
} Ast;

Ast* Parse(Arena* arena, const char* program, Tokens* tokens, Error* error);

#endif // COMPILER_PARSER_H_
