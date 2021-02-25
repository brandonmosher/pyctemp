#include "stdio.h"
#include "map.int.int.h"

size_t hash(int i) { return (size_t)i;}
int match(int i, int j) { return i == j;}

int main() {
    map_int_int_t map;
    map_int_int_init(&map, 10, hash, match);
    for(int i = 0; i < 10; ++i) {
        int* p = map_int_int_insert(&map, i, (i - 1));
    }
    
    for(int i = 0; i < 10; ++i) {
        int* r = map_int_int_find(&map, i);
        printf("%i\n", *r);
    }
}