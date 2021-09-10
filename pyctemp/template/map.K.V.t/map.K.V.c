#include "map.K.V.h"

void map_K_V_init(map_K_V_t* map_K_V, 
                size_t num_pairs,
                size_t(*hash)(const K P),
                int(*match)(const K P, const K P))
{
    map_K_V->hash = hash;
    map_K_V->match = match;
    array_pair_K_V_t* pairs = &(map_K_V->pairs);
    array_pair_K_V_init(pairs, num_pairs);

    for(size_t i = 0; i < num_pairs; ++i) {
        array_pair_K_V_push_back(pairs, (pair_K_V_t){0});
    }
}

void map_K_V_destroy(map_K_V_t* map_K_V) {
    array_pair_K_V_destroy(&(map_K_V->pairs));
}

static size_t map_K_V_capacity(const map_K_V_t* map_K_V) {
    return array_pair_K_V_size(&(map_K_V->pairs));
}

static size_t map_K_V_hash_R(const map_K_V_t* map_K_V, K P key) {
    return (map_K_V->hash_R(key) % map_K_V_capacity(map_K_V));
}

static int map_K_V_match_R(const map_K_V_t* map_K_V, K P a, K P b) {
    return map_K_V->match_R(a, b);
}

static pair_K_V_t* map_K_V_find_pair_R(const map_K_V_t* map_K_V, K P key) {
    const array_pair_K_V_t* pairs = &(map_K_V->pairs);
    pair_K_V_t* pair = array_pair_K_V_begin(pairs) + map_K_V_hash_R(map_K_V, key);
    pair_K_V_t* end = array_pair_K_V_end(pairs);
    for(; pair < end; ++pair) {
        if(pair->valid && map_K_V_match_R(map_K_V, A(pair->key), key)) {
            return pair;
        }
    }
    return NULL;
}

V * map_K_V_find_R(const map_K_V_t* map_K_V, K P key) {
    pair_K_V_t* pair = map_K_V_find_pair_R(map_K_V, key);
    if(pair) {
        return &(pair->value);
    }
    return NULL;
}

static V * map_K_V_insert_pair(map_K_V_t* map_K_V, pair_K_V_t* new_pair);

static void map_K_V_grow(map_K_V_t* map_K_V) {
    map_K_V_t new_map;
    map_K_V_init(&new_map, 
                2*map_K_V_capacity(map_K_V),
                map_K_V->hash,
                map_K_V->match);
    
    pair_K_V_t* pair = array_pair_K_V_begin(&(map_K_V->pairs));
    pair_K_V_t* end = array_pair_K_V_end(&(map_K_V->pairs));
    for(; pair != end; ++pair) {
        map_K_V_insert_pair(&new_map, pair);
    }
    map_K_V_destroy(map_K_V);
    *map_K_V = new_map;
}

static V * map_K_V_insert_pair_R(map_K_V_t* map_K_V, pair_K_V_t* new_pair) {
    array_pair_K_V_t* pairs = &(map_K_V->pairs);
    pair_K_V_t* pair = array_pair_K_V_begin(pairs) + map_K_V_hash_R(map_K_V, new_pair->key);
    pair_K_V_t* end = array_pair_K_V_end(pairs);
    for(; pair < end; ++pair) {
        if(!pair->valid) {
            *pair = *new_pair;
            return &(pair->value);
        }
    }
    map_K_V_grow(map_K_V);
    return map_K_V_insert_pair_R(map_K_V, new_pair);
}

//TODO: 
V * map_K_V_insert_R(map_K_V_t* map_K_V, K P key, V P value) {
    pair_K_V_t* pair = map_K_V_find_pair_R(map_K_V, key);
    if(pair) {
        pair->value = P(value);
        return &(pair->value);
    }

    array_pair_K_V_t* pairs = &(map_K_V->pairs);
    pair = array_pair_K_V_begin(pairs) + map_K_V_hash_R(map_K_V, key);
    pair_K_V_t* end = array_pair_K_V_end(pairs);
    for(; pair < end; ++pair) {
        if(!pair->valid) {
            pair->valid = 1;
            pair->key = key;
            pair->value = P(value);
            return &(pair->value);
        }
    }
    map_K_V_grow(map_K_V);
    return map_K_V_insert_R(map_K_V, key, value);
}

V * map_K_V_find_insert_R(map_K_V_t* map_K_V, K P key, V P value) {
    V * ptr = map_K_V_find_R(map_K_V, key);
    if(ptr) {
        return ptr;
    }
    return map_K_V_insert_R(map_K_V, key, value);
}

void map_K_V_erase_R(map_K_V_t* map_K_V, K P key) {
    pair_K_V_t* pair = map_K_V_find_pair(map_K_V, key);
    if(pair) {
        pair->valid = 0;
    }
}
