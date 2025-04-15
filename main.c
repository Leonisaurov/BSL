#include <stdio.h>
#include "strings.h"

int main() {
    string_t string = from_str("Hello");

    printf("MSG: %s\n", string->str);

    free_string(string);
    return 0;
}
