#ifndef STRINGS_H
#define STRINGS_H
#include <stddef.h>

struct string {
    char* str;
    size_t len;
    size_t size;
};

typedef struct string* string_t;

/*
 * Strings returned by this library own a single allocation. Always release
 * them with free_string(); do not free string->str separately.
 * On failure the constructors return NULL and set errno.
 */

string_t from_str(const char* str);
string_t from_strn(const char* str, size_t len);

string_t repeat_char(char c, size_t r_times);
string_t repeat_str(const char* str, size_t r_times);
string_t repeat_strn(const char* str, size_t str_len, size_t r_times);

void free_string(string_t string);

#endif
