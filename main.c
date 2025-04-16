#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "animations.h"

void check(string_t res, const char* expected, const char* errmessage) {
    printf("MSG: %s\n", res->str);
    if(strcmp(res->str, expected) == 0)
        return;

    printf("Expected: %s\n", expected);
    printf("SEG: %s\n", errmessage);
    exit(EXIT_FAILURE);
}

int main() {
    string_t fstr = from_str("Hello");
    check(fstr, "Hello", "From STR");
    free_string(fstr);

    string_t frepeatc = repeat_char('a', 5);
    check(frepeatc, "aaaaa", "Repeat CHAR");
    free_string(frepeatc);

    string_t frepeats = repeat_str("HOLA", 5);
    check(frepeats, "HOLAHOLAHOLAHOLAHOLA", "Repeat STR");
    free_string(frepeats);

    string_t fcyclic_shift = from_str("HELLO");
    cyclic_shift(fcyclic_shift);
    check(fcyclic_shift, "ELLOH", "Cyclic Shift");
    free_string(fcyclic_shift);

    return 0;
}
