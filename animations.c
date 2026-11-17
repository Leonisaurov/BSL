#include "animations.h"

#include <errno.h>
#include <string.h>

bool cyclic_shift(string_t string) {
    if (string == NULL || string->str == NULL) {
        errno = EINVAL;
        return false;
    }

    size_t len = string->len;
    if (len > string->size) {
        errno = ERANGE;
        return false;
    }

    if (len <= 1)
        return true;

    char first = string->str[0];
    memmove(string->str, string->str + 1, len - 1);
    string->str[len - 1] = first;

    return true;
}