#include "strings.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Allocate a string with room for `capacity` bytes plus the terminating NUL.
 * The struct and the character buffer share one allocation so that each
 * string needs only one malloc/free pair.
 */
static string_t allocate_string(size_t capacity) {
    if (capacity > SIZE_MAX - sizeof(struct string) - 1) {
        errno = ERANGE;
        return NULL;
    }

    size_t allocation_size = sizeof(struct string) + capacity + 1;
    string_t string = (string_t) malloc(allocation_size);
    if (string == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    string->str = (char*) (string + 1);
    string->str[0] = '\0';
    string->len = 0;
    string->size = capacity;

    return string;
}

string_t from_strn(const char* str, size_t len) {
    if (str == NULL && len > 0) {
        errno = EINVAL;
        return NULL;
    }

    string_t string = allocate_string(len);
    if (string == NULL)
        return NULL;

    if (len > 0)
        memcpy(string->str, str, len);

    string->str[len] = '\0';
    string->len = len;

    return string;
}

string_t from_str(const char* str) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return from_strn(str, strlen(str));
}

string_t repeat_char(char c, size_t r_times) {
    string_t string = allocate_string(r_times);
    if (string == NULL)
        return NULL;

    if (r_times > 0)
        memset(string->str, c, r_times);

    string->str[r_times] = '\0';
    string->len = r_times;

    return string;
}

string_t repeat_strn(const char* str, size_t str_len, size_t r_times) {
    if (str == NULL && str_len > 0) {
        errno = EINVAL;
        return NULL;
    }

    if (r_times != 0 && str_len > SIZE_MAX / r_times) {
        errno = ERANGE;
        return NULL;
    }

    size_t total_len = str_len * r_times;

    string_t string = allocate_string(total_len);
    if (string == NULL)
        return NULL;

    if (total_len > 0) {
        memcpy(string->str, str, str_len);

        size_t written = str_len;
        while (written < total_len) {
            size_t chunk = total_len - written;
            if (chunk > written)
                chunk = written;

            /* chunk <= written, so source and destination do not overlap. */
            memcpy(string->str + written, string->str, chunk);
            written += chunk;
        }
    }

    string->str[total_len] = '\0';
    string->len = total_len;

    return string;
}

string_t repeat_str(const char* str, size_t r_times) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return repeat_strn(str, strlen(str), r_times);
}

void free_string(string_t string) {
    free(string);
}