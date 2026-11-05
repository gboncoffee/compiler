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
    union {
        Array array;
    } this;

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

typedef enum {
    ExpressionLiteral,
    ExpressionName,
    ExpressionOperation,
} ExpressionKind;

typedef struct {
    union {
        Literal   literal;
        String    name;
        Operation operation;
    } this;

    ExpressionKind kind;
} Expression;

struct Statement;

typedef struct CompoundStatement {
    struct Statement* this;
    struct CompoundStatement* next;
} CompoundStatement;

typedef enum {
    StatementAssignment,
    StatementIf,
    StatementWhile,
    StatementCompound,
} StatementKind;

typedef struct {
    String     lvalue;
    Expression rvalue;
} Assignment;

typedef struct {
    Expression        condition;
    struct Statement* body;
    struct Statement* elsee;
} If;

typedef struct {
    Expression condition;
} While;

typedef struct {
    union {
        Assignment        assignment;
        If                iff;
        While             whilee;
        CompoundStatement compound;
    } this;

    StatementKind kind;
} Statement;

typedef struct {
    String name;
    Type   type;
} Declaration;

typedef struct Declarations {
    Declaration* this;
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
    Statement  body;
} Subprogram;

typedef struct Subprograms {
    Subprogram* this;
    struct Subprograms* next;
} Subprograms;

typedef struct Ast {
    String        name;
    Declarations* declarations;
    Subprograms*  subprograms;
    Statement     main;
} Ast;

Ast* Parse(Arena* arena, const char* program, Tokens* tokens, Error* error);

#endif // COMPILER_PARSER_H_
