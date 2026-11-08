#include "parser.h"
#include "tokenizer.h"

Location MakeLocation(const char* program, Tokens* token)
{
    auto location = (Location) {1, 1};

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

Tokens* SkipIODeclaration(Tokens* tokens, const char* program, Error* error)
{
    if (tokens == nullptr || tokens->this.kind != TokenOpenParen)
        return tokens;
    tokens = tokens->next;
    if (tokens == nullptr || tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected IO declaration",
            .dynamic  = nullptr,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens == nullptr || tokens->this.kind != TokenComma) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected comma",
            .dynamic  = nullptr,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens == nullptr || tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected IO declaration",
            .dynamic  = nullptr,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens == nullptr || tokens->this.kind != TokenCloseParen) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected closing parenthesis",
            .dynamic  = nullptr,
        };
        return nullptr;
    }

    return tokens->next;
}

Declarations* ParseVariableDeclarations(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    (void) arena;
    (void) program;
    (void) tokens;
    (void) error;
    (void) tokensAfter;
}

Subprograms* ParseSubprograms(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    (void) arena;
    (void) program;
    (void) tokens;
    (void) error;
    *tokensAfter = tokens;
    return nullptr;
}

Statement ParseStatement(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    (void) arena;
    (void) program;
    (void) tokens;
    (void) error;
    *tokensAfter = tokens;
    return (Statement) {0};
}

void ParseProgramBody(
    Arena*      arena,
    Ast*        ast,
    const char* program,
    Tokens*     tokens,
    Error*      error
)
{
    if (tokens == nullptr) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected at least main program declaration",
            .dynamic  = nullptr,
        };
        return;
    }

    Tokens* tokensAfterDecl = nullptr;
    ast->declarations       = ParseVariableDeclarations(
        arena, program, tokens, error, &tokensAfterDecl
    );
    if (!ErrorOk(*error))
        return;

    Tokens* tokensAfterSubprgs = nullptr;
    ast->subprograms           = ParseSubprograms(
        arena, program, tokensAfterDecl, error, &tokensAfterSubprgs
    );
    if (!ErrorOk(*error))
        return;

    Tokens* tokensAfterMainPrg = nullptr;
    ast->main                  = ParseStatement(
        arena, program, tokensAfterSubprgs, error, &tokensAfterMainPrg
    );
    if (!ErrorOk(*error))
        return;

    if (tokensAfterMainPrg == nullptr ||
        tokensAfterMainPrg->this.kind != TokenPeriod) {
        *error = (Error) {
            .location = MakeLocation(program, nullptr),
            .message  = "expected . after main program declaration",
            .dynamic  = nullptr,
        };
        return;
    }
    if (tokensAfterMainPrg->next == nullptr ||
        tokensAfterMainPrg->next->this.kind != TokenEOF) {
        *error = (Error) {
            .location = MakeLocation(program, nullptr),
            .message  = "expected nothing, got something",
            .dynamic  = nullptr,
        };
        return;
    }
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

    tokens = SkipIODeclaration(tokens->next, program, error);
    if (!ErrorOk(*error))
        return nullptr;
    if (tokens == nullptr || tokens->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = "expected semicolon",
            .dynamic  = nullptr,
        };
        return nullptr;
    }

    ParseProgramBody(arena, ast, program, tokens, error);
    return ast;
}

Ast* Parse(Arena* arena, const char* program, Tokens* tokens, Error* error)
{
    tokens = SkipComments(tokens);

    return ParseProgram(arena, program, tokens, error);
}
