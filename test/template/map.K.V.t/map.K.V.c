#include "map.K.V.h"

void map_K_V_init(map_K_V_t* map, 
                size_t num_buckets,
                size_t(*hash)(K),
                int(*match)(K, K),
                K(*default_key)(void),
                V(*default_val)(void))
{
    map->hash = hash;
    map->match = match;
    map->default_key = default_key;
    map->default_val = default_val;
    array_bucket_K_V_t* buckets = &(map->buckets);
    array_bucket_K_V_init(buckets, num_buckets);

    bucket_K_V_t default_bucket = (bucket_K_V_t){1, map->default_key(), map->default_val()};

    for(int i = 0; i < num_buckets; ++i) {
        array_bucket_K_V_push_back(buckets, default_bucket);
    }
}

void map_K_V_destroy(map_K_V_t* map) {
    array_bucket_K_V_destroy(&(map->buckets));
}

size_t map_K_V_num_buckets(map_K_V_t* map) {
    return array_bucket_K_V_size(&(map->buckets));
}

size_t map_K_V_hash(map_K_V_t* map, K key) {
    return (map->hash(key) % map_K_V_num_buckets(map));
}

int map_K_V_match(map_K_V_t* map, K a, K b) {
    return map->match(a, b);
}

bucket_K_V_t* map_K_V_find(map_K_V_t* map, K key) {
    array_bucket_K_V_t* buckets = &(map->buckets);
    bucket_K_V_t* bucket = array_bucket_K_V_begin(buckets) + map_K_V_hash(map, key);
    bucket_K_V_t* end = array_bucket_K_V_end(buckets);
    for(; bucket < end; ++bucket) {
        if(!bucket->empty && map_K_V_match(map, bucket->key, key)) {
            return bucket;
        }
    }
    return NULL;
}

static V * map_K_V_insert_bucket(map_K_V_t* map, bucket_K_V_t* new_bucket);

void map_K_V_grow(map_K_V_t* map) {
    map_K_V_t new_map;
    map_K_V_init(&new_map, 
                2*map_K_V_num_buckets(map),
                map->hash,
                map->match,
                map->default_key,
                map->default_val);
    bucket_K_V_t* bucket = array_bucket_K_V_begin(&(map->buckets));
    bucket_K_V_t* end = array_bucket_K_V_end(&(map->buckets));
    for(; bucket != end; ++bucket) {
        map_K_V_insert_bucket(&new_map, bucket);
    }
    map_K_V_destroy(map);
    *map = new_map;
}

static V * map_K_V_insert_bucket(map_K_V_t* map, bucket_K_V_t* new_bucket) {
    array_bucket_K_V_t* buckets = &(map->buckets);
    bucket_K_V_t* bucket = array_bucket_K_V_begin(buckets) + map_K_V_hash(map, new_bucket->key);
    bucket_K_V_t* end = array_bucket_K_V_end(buckets);
    for(; bucket < end; ++bucket) {
        if(bucket->empty) {
            *bucket = *new_bucket;
            return &(bucket->val);
        }
    }
    map_K_V_grow(map);
    return map_K_V_insert_bucket(map, new_bucket);
}

V* map_K_V_insert_R(map_K_V_t* map, K key, V P val) {
    array_bucket_K_V_t* buckets = &(map->buckets);
    bucket_K_V_t* bucket = array_bucket_K_V_begin(buckets) + map_K_V_hash(map, key);
    bucket_K_V_t* end = array_bucket_K_V_end(buckets);
    for(; bucket < end; ++bucket) {
        if(bucket->empty) {
            bucket->empty = 0;
            bucket->key = key;
            bucket->val = P(val);
            return &(bucket->val);
        }
    }
    map_K_V_grow(map);
    return map_K_V_insert_R(map, key, val);
}

V* map_K_V_get(map_K_V_t* map, K key) {
    bucket_K_V_t* bucket = map_K_V_find(map, key);
    if(bucket) {
        return &(bucket->val);
    }
    else if(map->default_val) {
        return map_K_V_insert(map, key, map->default_val());
    }
    else {
        return NULL;
    }
}

void map_K_V_erase(map_K_V_t* map, K key) {
    bucket_K_V_t* bucket = map_K_V_find(map, key);
    if(bucket) {
        bucket->empty = 1;
    }
}