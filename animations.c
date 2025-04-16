#include "animations.h"
// #include <stdio.h>

bool cyclic_shift(string_t string) {
    if (string == NULL)
        return false;

    char *str = string->str;
    char init = *str;

    size_t i = 0;
    while(str[i] != '\0') {
        // printf("%lu: %c->%c;str[%lu]=%c\n", i, str[i], str[i + 1], i, str[i + 1]);
        str[i] = str[i + 1];
        i++;
    }
    i--;
    // printf("%lu: %c->%c;str[%lu]=%c\n", i, str[i], init, i, init);
    str[i] = init;

    return true;
}
