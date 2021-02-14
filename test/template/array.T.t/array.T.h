
#ifndef ARRAY_T_H
#define ARRAY_T_H

#include <unistd.h>

#define T int
#define P *
#define A &

typedef T _T;

typedef struct {
    size_t size;
    size_t capacity;
    T* els;
} array_T_t;

void array_T_init(array_T_t* array, size_t capacity);

void array_T_destroy(array_T_t* array);

void array_T_clear(array_T_t* array);

size_t array_T_capacity(const array_T_t* array);

size_t array_T_size(const array_T_t* array);

int array_T_empty(const array_T_t* array);

int array_T_full(const array_T_t* array);

int array_T_inbounds(const array_T_t* array, size_t i);

T* array_T_begin(const array_T_t* array);

T* array_T_end(const array_T_t* array);

T array_T_get(const array_T_t* array, size_t i);

T array_T_front(const array_T_t* array);

T array_T_back(const array_T_t* array);

void array_T_set_R(array_T_t* array, size_t i, T P el);

void array_T_push_back_R(array_T_t* array, T P el);

void array_T_pop_back(array_T_t* array);

void array_T_grow(array_T_t* array);

#endif
