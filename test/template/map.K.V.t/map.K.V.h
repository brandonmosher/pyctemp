#ifndef MAP_K_V_PB_H
#define MAP_K_V_PB_H

#define K int
#define V int
#define P *
#define A &

#include "bucket.K.V.h"
#include "array.bucket_K_V_t.h"

typedef struct {
    array_bucket_K_V_t buckets;
    size_t(*hash)(K);
    int(*match)(K, K);
    K(*default_key)(void);
    V(*default_val)(void);
} map_K_V_t;

void map_K_V_init(map_K_V_t* map, 
                size_t num_buckets,
                size_t(*hash)(K),
                int(*match)(K, K),
                K(*default_key)(void),
                V(*default_val)(void));

void map_K_V_destroy(map_K_V_t* map);

size_t map_K_V_num_buckets(map_K_V_t* map);

size_t map_K_V_hash(map_K_V_t* map, K key);

int map_K_V_match(map_K_V_t* map, K a, K b);

bucket_K_V_t* map_K_V_find(map_K_V_t* map, K key);

V* map_K_V_insert_R(map_K_V_t* map, K key, V P value);

void map_K_V_grow(map_K_V_t* map);

V* map_K_V_get(map_K_V_t* map, K key);

void map_K_V_erase(map_K_V_t* map, K key);

#endif