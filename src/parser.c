#include "parser.h"

Ast* Parse(Arena* arena, Tokens* tokens, Errors** errors)
{
    for (auto token = tokens; token != nullptr; token = token->next) {
        switch (token->this.kind) {
        case TokenProgram:
            printf("TokenProgram\n");
            break;
        case TokenVar:
            printf("TokenVar\n");
            break;
        case TokenIdent:
            printf(
                "TokenIdent %.*s\n",
                (int) token->this.lex.size,
                token->this.lex.ptr
            );
            break;
        case TokenDigit:
            printf(
                "TokenDigit %.*s\n",
                (int) token->this.lex.size,
                token->this.lex.ptr
            );
            break;
        case TokenArray:
            printf("TokenArray\n");
            break;
        case TokenOf:
            printf("TokenOf\n");
            break;
        case TokenOpenSq:
            printf("TokenOpenSq\n");
            break;
        case TokenCloseSq:
            printf("TokenCloseSq\n");
            break;
        case TokenOpenParen:
            printf("TokenOpenParen\n");
            break;
        case TokenCloseParen:
            printf("TokenCloseParen\n");
            break;
        case TokenComma:
            printf("TokenComma\n");
            break;
        case TokenColon:
            printf("TokenColon\n");
            break;
        case TokenSemicolon:
            printf("TokenSemicolon\n");
            break;
        case TokenPeriod:
            printf("TokenPeriod\n");
            break;
        case TokenTwoPeriods:
            printf("TokenTwoPeriods\n");
            break;
        case TokenOperator:
            printf(
                "TokenOperator %.*s\n",
                (int) token->this.lex.size,
                token->this.lex.ptr
            );
            break;
        case TokenIf:
            printf("TokenIf\n");
            break;
        case TokenElse:
            printf("TokenElse\n");
            break;
        case TokenThen:
            printf("TokenThen\n");
            break;
        case TokenWhile:
            printf("TokenWhile\n");
            break;
        case TokenDo:
            printf("TokenDo\n");
            break;
        case TokenBegin:
            printf("TokenBegin\n");
            break;
        case TokenEnd:
            printf("TokenEnd\n");
            break;
        case TokenFunction:
            printf("TokenFunction\n");
            break;
        case TokenProcedure:
            printf("TokenProcedure\n");
            break;
        case TokenComment:
            printf("TokenComment\n");
            break;
        case TokenEOF:
            printf("TokenEOF\n");
            break;
        }
    }

    (void) arena;
    (void) errors;
    return nullptr;
}
