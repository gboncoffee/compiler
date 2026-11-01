#ifndef COMPILER_ARENA_H_
#define COMPILER_ARENA_H_

typedef struct Arena {
    struct Arena* next;
    unsigned long size;
    unsigned long used;
    char          heap[];
} Arena;

Arena* NewArena(long size);
void   DropArena(Arena* a);
void*  Calloc(Arena* a, unsigned long size);

#endif // COMPILER_ARENA_H_
