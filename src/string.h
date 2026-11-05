#ifndef COMPILER_STRING_H_
#define COMPILER_STRING_H_

typedef struct {
    const char*   ptr;
    unsigned long size;
} String;

bool StringEq(String a, String b);
bool StringEqCstr(String a, const char* cstr);

#endif // COMPILER_STRING_H_
