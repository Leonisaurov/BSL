#ifndef STRINGS_H
#define STRINGS_H
#include <stddef.h>

struct string {
    char* str;
    size_t len;
    size_t size;
};

typedef struct string* string_t;

string_t from_str(const char* str);

string_t repeat_char(char c, unsigned short r_times);
string_t repeat_str(const char* str, unsigned short r_times);

void free_string(string_t string);

#endif
