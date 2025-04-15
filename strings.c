#include "strings.h"
#include <string.h>
#include <malloc.h>

size_t utf8_string_len(const char* str) {
    size_t size = 0;
    if (str == NULL)
        return -1;

    while (str[size] != '\0') {
        size++;
    }

    return size;
}

string_t new_string() {
    string_t string = (string_t) malloc(sizeof(struct string));
    if (string == NULL)
        return NULL;

    string->str = NULL;
    string->len = 0;
    string->size = 0;

    return string;
}

string_t from_str(const char* str) {
    string_t string = new_string();
    if (string == NULL)
        return NULL;

    size_t str_len = utf8_string_len(str);
    string->size = str_len * 1.5;
    string->str = (char*) malloc(sizeof(char) * (string->size + 1));
    if (string->str == NULL) {
        free_string(string);
        return NULL;
    }
    memcpy(string->str, str, str_len);
    str_len = str_len;

    return string;
}

void free_string(string_t string) {
    if (string == NULL)
        return;
    if (string->str != NULL)
        free(string->str);
    free(string);
}
