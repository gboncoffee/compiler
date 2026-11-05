#include "string.h"

#include <string.h>

bool StringEqCstr(String a, const char* b)
{
    return ((strncmp(a.ptr, b, a.size) == 0) && strlen(b) == a.size);
}

bool StringEq(String a, String b)
{
    return a.size == b.size && memcmp(a.ptr, b.ptr, a.size) == 0;
}
