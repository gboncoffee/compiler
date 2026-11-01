#ifndef COMPILER_TOKENIZER_H_
#define COMPILER_TOKENIZER_H_

#include "arena.h"
#include "compiler.h"

typedef struct {
    const char*   ptr;
    unsigned long size;
} String;

typedef enum {
    TokenProgram,
    TokenVar,
    TokenIdent,
    TokenDigit,
    TokenArray,
    TokenOf,
    TokenOpenSq,
    TokenCloseSq,
    TokenOpenParen,
    TokenCloseParen,
    TokenComma,
    TokenColon,
    TokenSemicolon,
    TokenPeriod,
    TokenTwoPeriods,
    TokenOperator,
    TokenIf,
    TokenElse,
    TokenThen,
    TokenWhile,
    TokenDo,
    TokenBegin,
    TokenEnd,
    TokenFunction,
    TokenProcedure,
    TokenComment,
    TokenEOF,
} TokenKind;

typedef struct {
    String    lex;
    TokenKind kind;
} Token;

typedef struct Tokens {
    Token this;
    struct Tokens* next;
} Tokens;

Tokens* Tokenize(Arena* arena, char* program, Errors** errors);

#endif // COMPILER_TOKENIZER_H_
