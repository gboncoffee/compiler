#ifndef COMPILER_STRING_H_
#define COMPILER_STRING_H_

typedef struct {
    const char*   ptr;
    unsigned long size;
} String;

#define FROM_STR(str) ((String) {.ptr = str, .size = strlen(str)})
#define STR(str)      ((String) {.ptr = str, .size = sizeof(str) - 1})

bool StringEq(String a, String b);
bool StringEqCstr(String a, const char* cstr);

#endif // COMPILER_STRING_H_
