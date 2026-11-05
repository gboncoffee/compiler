#include "parser.h"
#include "tokenizer.h"

Location MakeLocation(const char* program, Tokens* token)
{
    Location location;

    for (auto i = 0; program[i] != '\0'; i += 1) {
        if (token != nullptr && token->this.lex.ptr == &program[i])
            break;
        if (program[i] == '\n') {
            location.line += 1;
            location.column = 1;
        } else {
            location.column += 1;
        }
    }

    return location;
}

Tokens* SkipComments(Tokens* tokens)
{
    if (tokens == nullptr)
        return nullptr;
    if (tokens->this.kind == TokenComment)
        return SkipComments(tokens);
    tokens->next = SkipComments(tokens->next);
    return tokens;
}

Tokens* SkipIODeclaration(Tokens* tokens)
{
    if (tokens == nullptr)
        return nullptr;
    if (tokens->this.kind != TokenOpenParen)
        return tokens;
    tokens = tokens->next;
    return nullptr;
}

Ast* ParseRec(
    Arena*      arena,
    Ast*        ast,
    const char* program,
    Tokens*     tokens,
    Error*      error
)
{
    if (tokens == nullptr) {}
}

Ast* ParseProgram(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error
)
{
    if (tokens == nullptr || tokens->this.kind != TokenProgram) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected program declaration",
            .dynamic  = nullptr,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens == nullptr || tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected program name",
            .dynamic  = nullptr,
        };
    }

    auto ast          = (Ast*) Calloc(arena, sizeof(Ast));
    ast->name         = tokens->this.lex;
    ast->declarations = nullptr;
    ast->subprograms  = nullptr;

    tokens = SkipIODeclaration(tokens);
    if (tokens == nullptr || tokens->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected semicolon",
            .dynamic  = nullptr,
        };
        return nullptr;
    }

    return ParseRec(arena, ast, program, tokens, error);
}

Ast* Parse(Arena* arena, const char* program, Tokens* tokens, Error* error)
{
    tokens = SkipComments(tokens);

    return ParseProgram(arena, program, tokens, error);
}
