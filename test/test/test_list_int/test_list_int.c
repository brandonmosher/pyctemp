#include <stdio.h>
#include "list.int.h"

void int_print(const int i) {
    printf("%i\n", i);
}

int int_sort(const int a, const int b) {
    return a < b;
}

int main() {
    list_int_t list_int;
    
    list_int_init(&list_int);
    
    for(int i = 0; i < 10; ++i) {
        list_int_push_back(&list_int, i);
    }

    for(int i = 10; i > 0; --i) {
        list_int_push_back(&list_int, i);
    }

    printf("\nUnsorted:\n");
    list_int_for_each(&list_int, int_print);

    list_int_bubble_sort(&list_int, int_sort);

    printf("\nSorted:\n");
    list_int_for_each(&list_int, int_print);

    list_int_destroy(&list_int);
}