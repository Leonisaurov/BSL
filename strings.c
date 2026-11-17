#include "strings.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static string_t new_string(void);

/*
 * Allocate a string with room for `capacity` bytes plus the terminating NUL.
 * The initial contents are an empty string.
 */
static string_t allocate_string(size_t capacity) {
    if (capacity == SIZE_MAX)
        return NULL;

    string_t string = new_string();
    if (string == NULL)
        return NULL;

    string->str = (char*) malloc(capacity + 1);
    if (string->str == NULL) {
        free_string(string);
        return NULL;
    }

    string->str[0] = '\0';
    string->size = capacity;
    return string;
}

static string_t new_string(void) {
    string_t string = (string_t) malloc(sizeof(struct string));
    if (string == NULL)
        return NULL;

    string->str = NULL;
    string->len = 0;
    string->size = 0;

    return string;
}

string_t from_str(const char* str) {
    if (str == NULL)
        return NULL;

    size_t str_len = strlen(str);

    string_t string = allocate_string(str_len);
    if (string == NULL)
        return NULL;

    if (str_len > 0)
        memcpy(string->str, str, str_len);

    string->str[str_len] = '\0';
    string->len = str_len;

    return string;
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

string_t repeat_str(const char* str, size_t r_times) {
    if (str == NULL)
        return NULL;

    size_t str_len = strlen(str);

    if (r_times != 0 && str_len > SIZE_MAX / r_times)
        return NULL;

    size_t total_len = str_len * r_times;

    string_t string = allocate_string(total_len);
    if (string == NULL)
        return NULL;

    if (total_len > 0) {
        for (size_t i = 0; i < r_times; i++)
            memcpy(string->str + i * str_len, str, str_len);
    }

    string->str[total_len] = '\0';
    string->len = total_len;

    return string;
}

void free_string(string_t string) {
    if (string == NULL)
        return;

    if (string->str != NULL)
        free(string->str);
    free(string);
}
