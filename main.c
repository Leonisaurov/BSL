#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "animations.h"

static void fail(const char* message) {
    fprintf(stderr, "FAIL: %s\n", message);
    exit(EXIT_FAILURE);
}

static void check(string_t res, const char* expected, const char* errmessage) {
    if (res == NULL)
        fail(errmessage);
    if (res->str == NULL) {
        fprintf(stderr, "FAIL: %s: string->str is NULL\n", errmessage);
        exit(EXIT_FAILURE);
    }

    printf("MSG: %s\n", res->str);

    if (strcmp(res->str, expected) != 0) {
        fprintf(stderr, "FAIL: %s\nExpected: %s\nActual:   %s\n",
                errmessage, expected, res->str);
        exit(EXIT_FAILURE);
    }
}

static void check_null(string_t res, const char* errmessage) {
    if (res != NULL) {
        fprintf(stderr, "FAIL: %s: expected NULL, got \"%s\"\n",
                errmessage, res->str ? res->str : "");
        free_string(res);
        exit(EXIT_FAILURE);
    }

    printf("MSG: %s: NULL as expected\n", errmessage);
}

static void check_length(string_t res, size_t expected, const char* errmessage) {
    if (res == NULL)
        fail(errmessage);
    if (res->str == NULL) {
        fprintf(stderr, "FAIL: %s: string->str is NULL\n", errmessage);
        exit(EXIT_FAILURE);
    }

    if (res->len != expected || res->str[expected] != '\0') {
        fprintf(stderr, "FAIL: %s\nExpected length: %zu\nActual length:   %zu\n",
                errmessage, expected, res->len);
        exit(EXIT_FAILURE);
    }

    printf("MSG: %s: length %zu\n", errmessage, res->len);
}

static void check_shift(string_t value, const char* expected, const char* errmessage) {
    if (!cyclic_shift(value))
        fail(errmessage);
    check(value, expected, errmessage);
}

int main(void) {
    string_t fstr = from_str("Hello");
    check(fstr, "Hello", "From STR");
    free_string(fstr);

    string_t fempty = from_str("");
    check(fempty, "", "From empty STR");
    free_string(fempty);

    check_null(from_str(NULL), "From NULL STR");

    string_t frepeatc = repeat_char('a', 5);
    check(frepeatc, "aaaaa", "Repeat CHAR");
    free_string(frepeatc);

    string_t frepeatc0 = repeat_char('a', 0);
    check(frepeatc0, "", "Repeat CHAR zero times");
    free_string(frepeatc0);

    string_t frepeatc_large = repeat_char('x', 70000);
    check_length(frepeatc_large, 70000, "Repeat CHAR large");
    free_string(frepeatc_large);

    string_t frepeats = repeat_str("HOLA", 5);
    check(frepeats, "HOLAHOLAHOLAHOLAHOLA", "Repeat STR");
    free_string(frepeats);

    string_t frepeats_empty = repeat_str("", 5);
    check(frepeats_empty, "", "Repeat empty STR");
    free_string(frepeats_empty);

    string_t frepeats_zero = repeat_str("HOLA", 0);
    check(frepeats_zero, "", "Repeat STR zero times");
    free_string(frepeats_zero);

    string_t frepeats_large = repeat_str("ab", 40000);
    check_length(frepeats_large, 80000, "Repeat STR large");
    free_string(frepeats_large);

    check_null(repeat_str(NULL, 5), "Repeat NULL STR");

    string_t fcyclic_shift = from_str("HELLO");
    check(fcyclic_shift, "HELLO", "Cyclic Shift init");
    check_shift(fcyclic_shift, "ELLOH", "Cyclic Shift");
    check_shift(fcyclic_shift, "LLOHE", "Cyclic Shift twice");
    free_string(fcyclic_shift);

    string_t fempty_shift = from_str("");
    if (!cyclic_shift(fempty_shift))
        fail("Cyclic Shift empty");
    check(fempty_shift, "", "Cyclic Shift empty");
    free_string(fempty_shift);

    string_t fone_shift = from_str("A");
    if (!cyclic_shift(fone_shift))
        fail("Cyclic Shift one char");
    check(fone_shift, "A", "Cyclic Shift one char");
    free_string(fone_shift);

    if (cyclic_shift(NULL))
        fail("Cyclic Shift NULL returned true");

    puts("All tests passed");
    return EXIT_SUCCESS;
}
