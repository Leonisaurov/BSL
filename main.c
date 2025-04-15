#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"

void check(string_t res, const char* expected, const char* errmessage) {
    printf("MSG: %s\n", res->str);
    printf("Expected: %s\n", expected);

    if(strcmp(res->str, expected) == 0) {
        puts("Match");
        return;
    }

    printf("%s:\n%s\n%s\n", errmessage, res->str, expected);
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

    return 0;
}
