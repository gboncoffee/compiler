#include "arena.h"
#include "compiler.h"
#include "parser.h"
#include "tokenizer.h"

#include <string.h>

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

Type ParseType(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
);

Array ParseArraySpecifier(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    auto array = (Array) {0};
    if (tokens->this.kind != TokenOpenSq) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected array size specifier, got"),
            .dynamic  = tokens->this.lex,
        };
        return array;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenDigit ||
        String2Long(tokens->this.lex, &array.begin)) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected number for array size specifier, got"),
            .dynamic  = tokens->this.lex,
        };
        return array;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenTwoPeriods) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message =
                STR("expected two periods for array size specifier, got"),
            .dynamic = tokens->this.lex,
        };
        return array;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenDigit ||
        String2Long(tokens->this.lex, &array.end)) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected number for array size specifier, got"),
            .dynamic  = tokens->this.lex,
        };
        return array;
    }
    tokens = tokens->next;
    if (tokens->this.kind != TokenCloseSq) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR(
                "expected closing square bracket for array size specifier, got"
            ),
            .dynamic = tokens->this.lex,
        };
        return array;
    }

    *tokensAfter = tokens->next;

    array.content = (struct Type*) Calloc(arena, sizeof(Type));
    auto type     = ParseType(arena, program, *tokensAfter, error, tokensAfter);
    memcpy(array.content, &type, sizeof(type));

    return array;
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
    (void) tokensAfter;
    return (Expression) {0};
}

Parameters* ParseParameterDeclarations(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind != TokenIdent && tokens->this.kind != TokenVar) {
        *tokensAfter = tokens;
        return nullptr;
    }

    auto parameters            = (Parameters*) Calloc(sizeof(Parameters));
    parameters->next           = nullptr;
    parameters->this.reference = tokens->this.kind == TokenVar;

    if (tokens->this.kind == TokenVar)
        tokens = tokens->next;

    parameters->this.this =
        ParseDeclaration(arena, program, tokens, error, tokensAfter);
    if ((*tokensAfter)->this.kind == TokenSemicolon)
        parameters->next = ParseParameterDeclarations(
            arena, program, (*tokensAfter)->next, tokensAfter
        );
    return parameters;
}

Parameters* ParseParameterList(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind != TokenOpenParen) {
        *tokensAfter = tokens;
        return nullptr;
    }

    auto parameters = ParseParameterDeclarations(
        arena, program, tokens->next, error, tokensAfter
    );

    if ((*tokensAfter)->this.kind != TokenCloseParen) {
        *error = (Error) {
            .location = MakeLocation(program, *tokensAfter),
            .message  = STR("expected ), got"),
            .dynamic  = (*tokensAfter)->this.lex,
        };
        return parameters;
    }
    *tokensAfter = (*tokensAfter)->next;

    return parameters;
}

Subprogram ParseFunction(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    auto prg = (Subprogram) {0};
    if (tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected function name, got"),
            .dynamic  = tokens->this.lex,
        };
        return prg;
    }

    prg.name = tokens->this.lex;
    prg.parameters =
        ParseParameters(arena, program, tokens, error, tokensAfter);
    if (!ErrorOk(*error))
        return prg;

    if ((*tokensAfter)->this.kind != TokenColon) {
        *error = (Error) {
            .location = MakeLocation(program, *tokensAfter),
            .message  = STR("expected return type, got"),
            .dynamic  = tokens->this.lex,
        };
        return prg;
    }
    prg.returnType =
        ParseType(arena, program, *tokensAfter, error, tokensAfter);
    if (!ErrorOk(*error))
        return prg;

    if ((*tokensAfter)->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, *tokensAfter),
            .message  = STR("expected semicolon, got"),
            .dynamic  = tokens->this.lex,
        };
        return prg;
    }
    prg.body =
        ParseExpression(arena, program, *tokensAfter, error, tokensAfter);
    if ((*tokensAfter)->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, *tokensAfter),
            .message  = STR("expected semicolon, got"),
            .dynamic  = tokens->this.lex,
        };
    }
    *tokensAfter = (*tokensAfter)->next;
    return prg;
}

Subprogram ParseProcedure(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    auto prg = (Subprogram) {
        .returnType = TypeVoid,
    };
    if (tokens->this.kind != TokenIdent) {
        *error = (Error) {
            .location = MakeLocation(program, tokens),
            .message  = STR("expected procedure name, got"),
            .dynamic  = tokens->this.lex,
        };
        return prg;
    }

    prg.name = tokens->this.lex;
    prg.parameters =
        ParseParameters(arena, program, tokens, error, tokensAfter);
    if (!ErrorOk(*error))
        return prg;

    if ((*tokensAfter)->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, *tokensAfter),
            .message  = STR("expected semicolon, got"),
            .dynamic  = tokens->this.lex,
        };
        return prg;
    }
    prg.body =
        ParseExpression(arena, program, *tokensAfter, error, tokensAfter);
    if ((*tokensAfter)->this.kind != TokenSemicolon) {
        *error = (Error) {
            .location = MakeLocation(program, *tokensAfter),
            .message  = STR("expected semicolon, got"),
            .dynamic  = tokens->this.lex,
        };
    }
    *tokensAfter = (*tokensAfter)->next;
    return prg;
}

Subprogram ParseSubprogram(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind == TokenFunction)
        return ParseFunction(arena, program, tokens->next, error, tokensAfter);
    return ParseProcedure(arena, program, tokens->next, error, tokensAfter);
}

Subprograms* ParseSubprograms(
    Arena*      arena,
    const char* program,
    Tokens*     tokens,
    Error*      error,
    Tokens**    tokensAfter
)
{
    if (tokens->this.kind != TokenProcedure &&
        tokens->this.kind != TokenFunction)
        return nullptr;

    auto prgs   = (Subprograms*) Calloc(arena, sizeof(Subprograms));
    *prgs->this = ParseSubprogram(arena, program, tokens, error, tokensAfter);
    if (!ErrorOk(*error))
        return nullptr;
    prgs->next =
        ParseSubprograms(arena, program, *tokensAfter, error, tokensAfter);
    return prgs;
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
