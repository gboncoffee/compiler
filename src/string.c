#include "string.h"

#include <stdckdint.h>
#include <string.h>

bool StringEqCstr(const String a, const char* b)
{
    return ((strncmp(a.ptr, b, a.size) == 0) && strlen(b) == a.size);
}

bool StringEq(const String a, const String b)
{
    return a.size == b.size && memcmp(a.ptr, b.ptr, a.size) == 0;
}

int String2Long(const String a, long* ret)
{
    unsigned long i   = 0;
    bool          neg = false;
    long          r   = 0;

    if (a.size == 0)
        return 1;
    if (a.ptr[0] == '-' || a.ptr[0] == '+') {
        neg = a.ptr[0] == '-';
        i   = 1;
        if (a.size == 1)
            return 1;
    }

    for (; i < a.size; i++) {
        if (a.ptr[i] < '0' || a.ptr[i] > '9')
            return 1;
        long d = a.ptr[i] - '0';
        if (ckd_mul(&r, r, 10) || (neg ? ckd_sub(&r, r, d) : ckd_add(&r, r, d)))
            return 1;
    }

    *ret = r;
    return 0;
}
