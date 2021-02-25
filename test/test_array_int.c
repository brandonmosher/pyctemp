#include <stdio.h>

#include "array.int.h"

int main() {
    array_int_t array;
    array_int_init(&array, 20);
    for(int i = 0; i < 10; ++i) {
        array_int_push_back(&array, i);
    }

    int* ptr = array_int_begin(&array);
    for(int i = 0; i < array_int_size(&array); ++i) {
        printf("%i\n", *ptr++);
    }
};