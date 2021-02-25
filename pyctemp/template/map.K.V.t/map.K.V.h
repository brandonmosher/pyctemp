#ifndef MAP_K_V_PB_H
#define MAP_K_V_PB_H

#define K int
#define V int
#define P *
#define A &

typedef K _K_t;
typedef V _V_t;

#include "pair.K.V.h"
#include "array.pair_K_V_t.h"

typedef struct {
    array_pair_K_V_t pairs;
    size_t(*hash)(K);
    int(*match)(K, K);
} map_K_V_t;

void map_K_V_init(map_K_V_t* map_K_V, 
                size_t num_pairs,
                size_t(*hash)(K),
                int(*match)(K, K));

void map_K_V_destroy(map_K_V_t* map_K_V);

V * map_K_V_find(const map_K_V_t* map_K_V, K key);

V * map_K_V_insert_R(map_K_V_t* map_K_V, K key, V P value);

V * map_K_V_find_insert_R(map_K_V_t* map_K_V, K key, V P value);

void map_K_V_erase(map_K_V_t* map_K_V, K key);

size_t map_K_V_size(const map_K_V_t* map_K_V);

#endif