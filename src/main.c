#include "arena.h"
#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

void Usage(const char* program)
{
    printf("usage: %s <input file>\n", program);
}

char* ReadFileOrDie(Arena* arena, const char* file)
{
    auto f = fopen(file, "r");
    if (f == NULL) {
        perror("Couldn't read input file");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    auto size = ftell(f);
    rewind(f);

    auto buffer = (char*) Calloc(arena, (size + 1) * sizeof(char));
    fread(buffer, 1, size, f);

    return buffer;
}

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        Usage(argv[0]);
        return 1;
    }

    auto arena = NewArena(32 * 4096);

    auto input = ReadFileOrDie(arena, argv[1]);

    FILE* output = fopen("a.out", "w");

    auto error = Compile(arena, input, output);
    if (!ErrorOk(error))
        PrintError(error, argv[1]);

    DropArena(arena);

    return 0;
}
