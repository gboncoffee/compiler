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
    if (tokens->this.kind == TokenEOF)
        return tokens;
    if (tokens->this.kind == TokenComment)
        return SkipComments(tokens);
    tokens->next = SkipComments(tokens->next);
    return tokens;
}

Tokens* SkipIODeclaration(Tokens* tokens, const char* program, Error* error)
{
    if (tokens->this.kind != TokenOpenParen)
        return tokens;
    tokens = tokens->next;
    if (tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected IO declaration, got"),
            .dynamic  = tokens->this.lex,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenComma) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected comma, got"),
            .dynamic  = tokens->this.lex,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected IO declaration, got"),
            .dynamic  = tokens->this.lex,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenCloseParen) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected closing parenthesis, got"),
            .dynamic  = tokens->this.lex,
        };
        return nullptr;
    }

    return tokens->next;
}

Names* ParseNameList(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected identifier, got"),
            .dynamic  = tokens->this.lex,
        };
        return nullptr;
    }

    auto name  = (Names*) Calloc(arena, sizeof(Names));
    name->this = tokens->this.lex;
    if (tokens->next == nullptr || tokens->next->this.kind != TokenComma) {
        *tokensAfter = tokens->next;
        name->next   = nullptr;
        return name;
    }
    name->next =
        ParseNameList(arena, program, tokens->next->next, error, tokensAfter);
    return name;
}

Array ParseArraySpecifier(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    (void) arena;
    (void) tokensAfter;
    if (tokens->this.kind != TokenOpenSq) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected array size specifier, got"),
            .dynamic  = tokens->this.lex,
        };
        return (Array) {0};
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenDigit) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected number for array size specifier, got"),
            .dynamic  = tokens->this.lex,
        };
        return (Array) {0};
    }
    // TODO.
}

Type ParseType(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    switch (tokens->this.kind) {
    case TokenReal:
        return (Type) {
            .kind = TypeReal,
        };
    case TokenInteger:
        return (Type) {
            .kind = TypeInteger,
        };
    case TokenArray:
        return (Type) {
            .kind  = TypeArray,
            .array = ParseArraySpecifier(
                arena, program, tokens->next, error, tokensAfter
            ),
        };
        break;
    default:
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected type, got"),
            .dynamic  = tokens->this.lex,
        };
        return (Type) {0};
    }
}

Type ParseTypeSpecifier(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind != TokenColon) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected colon after identifiers, got"),
            .dynamic  = tokens->this.lex,
        };
        return (Type) {0};
    }

    return ParseType(arena, program, tokens->next, error, tokensAfter);
}

Declaration ParseDeclaration(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    Declaration this;
    this.names = ParseNameList(arena, program, tokens, error, tokensAfter);
    if (!ErrorOk(*error))
        return this;
    this.type =
        ParseTypeSpecifier(arena, program, *tokensAfter, error, tokensAfter);
    return this;
}

Declarations* ParseVariableDeclarations(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind != TokenVar)
        return nullptr;

    auto decls = (Declarations*) Calloc(arena, sizeof(Declarations));
    decls->this =
        ParseDeclaration(arena, program, tokens->next, error, tokensAfter);
    if (!ErrorOk(*error))
        return nullptr;
    decls->next = ParseVariableDeclarations(
        arena, program, *tokensAfter, error, tokensAfter
    );
    return decls;
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

Expression ParseExpression(
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
    return (Expression) {0};
}

void ParseProgramBody(
    Arena*      arena,
    Ast*        ast,
    const char* program,
    Tokens*     tokens,
    Error*      error
)
{
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
    ast->main                  = ParseExpression(
        arena, program, tokensAfterSubprgs, error, &tokensAfterMainPrg
    );
    if (!ErrorOk(*error))
        return;

    if (tokensAfterMainPrg->this.kind != TokenPeriod) {
        *error = (Error) {
            .location = MakeLocation(program, nullptr),
            .message  = STR("expected . after main program declaration, got"),
            .dynamic  = tokens->this.lex,
        };
        return;
    }
    if (tokensAfterMainPrg->next->this.kind != TokenEOF) {
        *error = (Error) {
            .location = MakeLocation(program, nullptr),
            .message  = STR("expected nothing, got"),
            .dynamic  = tokens->this.lex,
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
    if (tokens->this.kind != TokenProgram) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected program declaration, got"),
            .dynamic  = tokens->this.lex,
        };
        return nullptr;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected program name, got"),
            .dynamic  = tokens->this.lex,
        };
    }

    auto ast          = (Ast*) Calloc(arena, sizeof(Ast));
    ast->name         = tokens->this.lex;
    ast->declarations = nullptr;
    ast->subprograms  = nullptr;

    tokens = SkipIODeclaration(tokens->next, program, error);
    if (!ErrorOk(*error))
        return nullptr;
    if (tokens->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected semicolon, got"),
            .dynamic  = tokens->this.lex,
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
