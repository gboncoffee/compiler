#include "arena.h"

#include <stdlib.h>
#include <string.h>

int constexpr BLOCK_ALIGNMENT = 16u;

static unsigned int AlignedSize(unsigned long size)
{
    return (size + BLOCK_ALIGNMENT - 1) & ~(BLOCK_ALIGNMENT - 1);
}

Arena* NewArena(long size)
{
    size = AlignedSize(size);

    Arena* arena = calloc(1, sizeof(*arena) + size);
    arena->size  = size;

    return arena;
}

void DropArena(Arena* arena)
{
    if (arena == nullptr)
        return;
    Arena* next = arena->next;
    free(arena);
    DropArena(next);
}

void* Calloc(Arena* arena, unsigned long size)
{
    size = AlignedSize(size);

    if (arena->size - arena->used < size) {
        if (arena->next == nullptr) {
            arena->next = NewArena(arena->size > size ? arena->size : size);
            arena       = arena->next;
        } else {
            Calloc(arena->next, size);
        }
    }

    void* ptr = (void*) &arena->heap[arena->used];
    memset(ptr, 0, size);
    arena->used += size;

    return ptr;
}
