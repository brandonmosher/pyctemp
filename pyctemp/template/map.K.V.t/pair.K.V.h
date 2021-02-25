#ifndef PAIR_K_V_H
#define PAIR_K_V_H

typedef K _K_t;
typedef V _V_t;

typedef struct {
    unsigned short valid : 1;
    K key;
    V value;
} pair_K_V_t;

#endif
