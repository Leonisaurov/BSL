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
    string->str[string->size] = '\0';
    if (string->str == NULL) {
        free_string(string);
        return NULL;
    }
    memcpy(string->str, str, str_len);
    string->len = str_len;
    string->str[string->len] = '\0';

    return string;
}

string_t repeat_char(char c, unsigned short r_times) {
    string_t string = new_string();
    if (string == NULL)
        return NULL;

    string->len = r_times;
    string->size = string->len * 1.5;
    string->str = (char*) malloc(sizeof(char) * (string->size + 1));
    if (string->str == NULL) {
        free_string(string);
        return NULL;
    }

    memset(string->str, c, string->len);
    string->str[string->len] = '\0';

    return string;
}

string_t repeat_str(const char* str, unsigned short r_times) {
    string_t string = new_string();
    if (string == NULL)
        return NULL;

    size_t str_len = utf8_string_len(str);
    string->len = str_len * r_times;
    string->size = string->len;
    string->str = (char*) malloc(sizeof(char) * (string->size + 1));
    if (string->str == NULL) {
        free_string(string);
        return NULL;
    }

    for (unsigned short i = 0; i < r_times; i++) {
        memcpy(string->str + i * str_len, str, str_len);
    }

    string->str[string->len] = '\0';

    return string;
}

void free_string(string_t string) {
    if (string == NULL)
        return;
    if (string->str != NULL)
        free(string->str);
    free(string);
}
