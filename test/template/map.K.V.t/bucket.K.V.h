#ifndef BUCKET_K_V_H
#define BUCKET_K_V_H

typedef struct {
    int empty : 1;
    K key;
    V val;
} bucket_K_V_t;

#endif
