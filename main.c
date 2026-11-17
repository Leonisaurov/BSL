#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
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

static void check_null_errno(string_t res, int expected_errno, const char* errmessage) {
    if (res != NULL) {
        fprintf(stderr, "FAIL: %s: expected NULL, got \"%s\"\n",
                errmessage, res->str ? res->str : "");
        free_string(res);
        exit(EXIT_FAILURE);
    }

    if (errno != expected_errno) {
        fprintf(stderr, "FAIL: %s\nExpected errno: %d\nActual errno:   %d\n",
                errmessage, expected_errno, errno);
        exit(EXIT_FAILURE);
    }

    printf("MSG: %s: NULL, errno=%d\n", errmessage, errno);
}

static void check_false_errno(bool ok, int expected_errno, const char* errmessage) {
    if (ok)
        fail(errmessage);

    if (errno != expected_errno) {
        fprintf(stderr, "FAIL: %s\nExpected errno: %d\nActual errno:   %d\n",
                errmessage, expected_errno, errno);
        exit(EXIT_FAILURE);
    }

    printf("MSG: %s: false, errno=%d\n", errmessage, errno);
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

static void check_bytes(string_t res, const char* expected, size_t expected_len,
                        const char* errmessage) {
    if (res == NULL)
        fail(errmessage);
    if (res->str == NULL) {
        fprintf(stderr, "FAIL: %s: string->str is NULL\n", errmessage);
        exit(EXIT_FAILURE);
    }

    if (res->len != expected_len || memcmp(res->str, expected, expected_len) != 0 ||
        res->str[expected_len] != '\0') {
        fprintf(stderr, "FAIL: %s\nExpected length: %zu\nActual length:   %zu\n",
                errmessage, expected_len, res->len);
        exit(EXIT_FAILURE);
    }

    printf("MSG: %s: %zu bytes\n", errmessage, res->len);
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

    string_t fstrn = from_strn("Hello", 5);
    check(fstrn, "Hello", "From STRN");
    free_string(fstrn);

    string_t fbinary = from_strn("a\0b", 3);
    check_bytes(fbinary, "a\0b", 3, "From STRN binary");
    free_string(fbinary);

    string_t fempty = from_str("");
    check(fempty, "", "From empty STR");
    free_string(fempty);

    string_t femptyn = from_strn(NULL, 0);
    check(femptyn, "", "From STRN NULL length zero");
    free_string(femptyn);

    errno = 0;
    check_null_errno(from_str(NULL), EINVAL, "From NULL STR");

    errno = 0;
    check_null_errno(from_strn(NULL, 1), EINVAL, "From STRN NULL length nonzero");

    string_t frepeatc = repeat_char('a', 5);
    check(frepeatc, "aaaaa", "Repeat CHAR");
    free_string(frepeatc);

    string_t frepeatc0 = repeat_char('a', 0);
    check(frepeatc0, "", "Repeat CHAR zero times");
    free_string(frepeatc0);

    string_t frepeatc_large = repeat_char('x', 70000);
    check_length(frepeatc_large, 70000, "Repeat CHAR large");
    free_string(frepeatc_large);

    errno = 0;
    check_null_errno(repeat_char('x', SIZE_MAX), ERANGE, "Repeat CHAR overflow");

    string_t frepeats = repeat_str("HOLA", 5);
    check(frepeats, "HOLAHOLAHOLAHOLAHOLA", "Repeat STR");
    free_string(frepeats);

    string_t frepeatsn = repeat_strn("ab", 2, 3);
    check(frepeatsn, "ababab", "Repeat STRN");
    free_string(frepeatsn);

    string_t frepeatsn_binary = repeat_strn("a\0b", 3, 2);
    check_bytes(frepeatsn_binary, "a\0ba\0b", 6, "Repeat STRN binary");
    free_string(frepeatsn_binary);

    string_t frepeats_empty = repeat_str("", 5);
    check(frepeats_empty, "", "Repeat empty STR");
    free_string(frepeats_empty);

    string_t frepeats_zero = repeat_str("HOLA", 0);
    check(frepeats_zero, "", "Repeat STR zero times");
    free_string(frepeats_zero);

    string_t frepeatsn_zero = repeat_strn("ab", 2, 0);
    check(frepeatsn_zero, "", "Repeat STRN zero times");
    free_string(frepeatsn_zero);

    string_t frepeatsn_null = repeat_strn(NULL, 0, 5);
    check(frepeatsn_null, "", "Repeat STRN NULL length zero");
    free_string(frepeatsn_null);

    string_t frepeats_large = repeat_str("ab", 40000);
    check_length(frepeats_large, 80000, "Repeat STR large");
    free_string(frepeats_large);

    string_t frepeatsn_large = repeat_strn("ab", 2, 40000);
    check_length(frepeatsn_large, 80000, "Repeat STRN large");
    free_string(frepeatsn_large);

    errno = 0;
    check_null_errno(repeat_str(NULL, 5), EINVAL, "Repeat NULL STR");

    errno = 0;
    check_null_errno(repeat_strn(NULL, 1, 5), EINVAL, "Repeat STRN NULL length nonzero");

    errno = 0;
    check_null_errno(repeat_strn("a", 1, SIZE_MAX), ERANGE, "Repeat STRN overflow");

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

    errno = 0;
    check_false_errno(cyclic_shift(NULL), EINVAL, "Cyclic Shift NULL");

    struct string null_string = {NULL, 0, 0};
    errno = 0;
    check_false_errno(cyclic_shift(&null_string), EINVAL, "Cyclic Shift null str");

    string_t fbad = from_str("A");
    fbad->len = fbad->size + 1;
    errno = 0;
    check_false_errno(cyclic_shift(fbad), ERANGE, "Cyclic Shift invalid length");
    free_string(fbad);

    free_string(NULL);

    puts("All tests passed");
    return EXIT_SUCCESS;
}