#include "array.T.h"

#include <stdlib.h>

void array_T_init(array_T_t* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->els = (T*)malloc(capacity * sizeof(T));
}

void array_T_destroy(array_T_t* array) {
    if(array->els) {
        free(array->els);
    }
    array->size = 0;
    array->capacity = 0;
}

void array_T_clear(array_T_t* array) {
    return array_T_destroy(array);
}

size_t array_T_capacity(const array_T_t* array) {
    return array->capacity;
}

size_t array_T_size(const array_T_t* array) {
    return array->size;
}

int array_T_empty(const array_T_t* array) {
    return (array->size == 0);
}

int array_T_full(const array_T_t* array) {
    return (array->size == array->capacity);
}

int array_T_inbounds(const array_T_t* array, size_t i) {
    return (i < array->size);
}

T* array_T_begin(const array_T_t* array) {
    return array->els;
}

T* array_T_end(const array_T_t* array) {
    return array->els + array->size;
}

T array_T_get(const array_T_t* array, size_t i) {
    return array->els[i];
}

T array_T_front(const array_T_t* array) {
    return array->els[0];
}

T array_T_back(const array_T_t* array) {
    return array->els[array->size - 1];
}

void array_T_set_R(array_T_t* array, size_t i, T P el) {
    array->els[i] = P(el);
}

void array_T_push_back_R(array_T_t* array, T P el) {
    if(array_T_full(array)) {
        array_T_grow(array);
    }
    array->els[(array->size)++] = P(el);
}

void array_T_pop_back(array_T_t* array) {
    --(array->size);
}

void array_T_grow(array_T_t* array) {
    T* els_new = (T*)malloc(sizeof(T) * array->size * 2);
    int i = 0;
    for(;i < array->size; ++i) {
        els_new[i] = array->els[i];
    }
    free(array->els);
    array->els = els_new;
    array->capacity = 2 * array->size;
}