#include "tokenizer.h"

#include <ctype.h>
#include <string.h>

typedef enum {
    StateInitial,
    StateAlpha,
    StateDigit,
    StateOpenParen,
    StateComment,
    StateClosingComment,
    StatePeriod,
    StateOperator,
} State;

typedef struct {
    const char* keyword;
    TokenKind   kind;
} Keyword;

static const Keyword keywords[] = {
    {  .keyword = "program",   .kind = TokenProgram},
    {      .keyword = "var",       .kind = TokenVar},
    {    .keyword = "array",     .kind = TokenArray},
    {       .keyword = "of",        .kind = TokenOf},
    {     .keyword = "then",      .kind = TokenThen},
    {    .keyword = "begin",     .kind = TokenBegin},
    {       .keyword = "do",        .kind = TokenDo},
    {    .keyword = "while",     .kind = TokenWhile},
    {      .keyword = "end",       .kind = TokenEnd},
    {       .keyword = "if",        .kind = TokenIf},
    {     .keyword = "else",      .kind = TokenElse},
    {     .keyword = "else",      .kind = TokenElse},
    { .keyword = "function",  .kind = TokenFunction},
    {.keyword = "procedure", .kind = TokenProcedure},
};

constexpr unsigned long keywordsLen = sizeof(keywords) / sizeof(keywords[0]);

#include <stdio.h>

Errors* MakeTokenError(
    Arena*      arena,
    const char* program,
    Token* this,
    const char* message,
    char*       dynamic
)
{
    int i      = 0;
    int line   = 1;
    int column = 1;
    while (&program[i] <= this->lex.ptr) {
        if (program[i] == '\n') {
            column = 0;
            line += 1;
        }
        i += 1;
        column += 1;
    }

    auto errors          = (Errors*) Calloc(arena, sizeof(Errors));
    errors->next         = nullptr;
    errors->this.line    = line;
    errors->this.column  = column;
    errors->this.message = message;
    errors->this.dynamic = dynamic;

    return errors;
}

void MakeKeyword(Token* this)
{
    for (auto i = 0; i < (int) keywordsLen; i += 1) {
        if ((strncmp(this->lex.ptr, keywords[i].keyword, this->lex.size) ==
             0) &&
            strlen(keywords[i].keyword) == this->lex.size) {
            this->kind = keywords[i].kind;
            return;
        }
    }

    this->kind = TokenIdent;
}

Tokens* Tokenize(Arena* arena, char* program, Errors** errors)
{
    (void) errors;
    auto list  = (Tokens*) Calloc(arena, sizeof(Tokens));
    auto token = list;

    for (int i = 0; program[i] != '\0'; i += 1) {
        program[i] = tolower(program[i]);
    }

    auto idx   = 0;
    auto state = StateInitial;
    for (;;) {
        const auto cchar = program[idx];
        const auto cint  = (int) (unsigned char) cchar;

        switch (state) {
        case StateInitial:
            if (cchar == '\0') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenEOF;
                token->next          = nullptr;
                return list;
            } else if (isalpha(cint)) {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                state                = StateAlpha;
                idx += 1;
            } else if (isdigit(cint)) {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                state                = StateDigit;
                idx += 1;
            } else if (cchar == ';') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenSemicolon;
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                idx += 1;
            } else if (cchar == ',') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenComma;
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                idx += 1;
            } else if (cchar == ':') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenColon;
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                idx += 1;
            } else if (cchar == '(') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                state                = StateOpenParen;
                idx += 1;
            } else if (cchar == ')') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenCloseParen;
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                idx += 1;
            } else if (cchar == '[') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenOpenSq;
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                idx += 1;
            } else if (cchar == ']') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                token->this.kind     = TokenCloseSq;
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                idx += 1;
            } else if (cchar == '.') {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                state                = StatePeriod;
                idx += 1;
            } else if (ispunct(cint)) {
                token->this.lex.ptr  = &program[idx];
                token->this.lex.size = 1;
                state                = StateOperator;
                idx += 1;
            } else {
                idx += 1;
            }
            break;
        case StateAlpha:
            if (isalnum(cint)) {
                token->this.lex.size += 1;
                idx += 1;
            } else {
                MakeKeyword(&token->this);
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token       = token->next;
                state       = StateInitial;
            }
            break;
        case StateDigit:
            if (isalnum(cint)) {
                token->this.lex.size += 1;
                idx += 1;
            } else {
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token->this.kind = TokenDigit;
                token            = token->next;
                state            = StateInitial;
            }
            break;
        case StateOpenParen:
            if (cchar == '*') {
                state = StateComment;
                token->this.lex.size += 1;
                idx += 1;
            } else {
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token->this.kind = TokenOpenParen;
                token            = token->next;
                state            = StateInitial;
            }
            break;
        case StateComment:
            if (cchar == '*') {
                state = StateClosingComment;
                token->this.lex.size += 1;
                idx += 1;

            } else if (cchar == '\0') {
                *errors = MakeTokenError(
                    arena, program, &token->this, "unclosed comment", nullptr
                );
                token->next      = nullptr;
                token->this.kind = TokenComment;
                return list;
            } else {
                idx += 1;
                token->this.lex.size += 1;
            }
            break;
        case StateClosingComment:
            if (cchar == ')') {
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token->this.lex.size += 1;
                token->this.kind = TokenComment;
                token            = token->next;
                state            = StateInitial;
                idx += 1;
            } else {
                idx += 1;
                token->this.lex.size += 1;
                state = StateComment;
            }
            break;
        case StatePeriod:
            if (cchar == '.') {
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token->this.lex.size += 1;
                token->this.kind = TokenTwoPeriods;
                token            = token->next;
                state            = StateInitial;
                idx += 1;
            } else {
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token->this.kind = TokenPeriod;
                token            = token->next;
                state            = StateInitial;
            }
            break;
        case StateOperator:
            if (ispunct(cint)) {
                token->this.lex.size += 1;
                idx += 1;
            } else {
                token->next = (Tokens*) Calloc(arena, sizeof(*token->next));
                token->this.kind = TokenOperator;
                token            = token->next;
                state            = StateInitial;
            }
            break;
        }
    }
}
