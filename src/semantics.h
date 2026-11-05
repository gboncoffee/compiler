#ifndef COMPILER_SEMANTICS_H_
#define COMPILER_SEMANTICS_H_

#include "arena.h"
#include "compiler.h"
#include "parser.h"

Error SemanticAnalysis(Arena* arena, Ast* tree);

#endif // COMPILER_SEMANTICS_H_
