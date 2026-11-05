#include "semantics.h"

#include <string.h>

Error SemanticAnalysis(Arena* arena, Ast* tree)
{
    Error error;
    memset(&error, 0, sizeof(error));
    (void) arena;
    (void) tree;
    return error;
}
