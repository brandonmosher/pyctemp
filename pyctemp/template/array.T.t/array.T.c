#include "array.T.h"

#include <stdlib.h>

void array_T_init(array_T_t* array_T, size_t capacity) {
    array_T->size = 0;
    array_T->capacity = capacity;
    array_T->values = (T*)malloc(capacity * sizeof(T));
}

void array_T_destroy(array_T_t* array_T) {
    if(array_T->values) {
        free(array_T->values);
    }
    array_T->size = 0;
    array_T->capacity = 0;
}

void array_T_clear(array_T_t* array_T) {
    array_T->size = 0;
}

size_t array_T_capacity(const array_T_t* array_T) {
    return array_T->capacity;
}

size_t array_T_size(const array_T_t* array_T) {
    return array_T->size;
}

int array_T_empty(const array_T_t* array_T) {
    return (array_T->size == 0);
}

int array_T_full(const array_T_t* array_T) {
    return (array_T->size == array_T->capacity);
}

int array_T_inbounds(const array_T_t* array_T, size_t index) {
    return (index < array_T->size);
}

T* array_T_begin(const array_T_t* array_T) {
    return array_T->values;
}

T* array_T_end(const array_T_t* array_T) {
    return array_T->values + array_T->size;
}

T array_T_get(const array_T_t* array_T, size_t index) {
    return array_T->values[index];
}

T array_T_front(const array_T_t* array_T) {
    return array_T->values[0];
}

T array_T_back(const array_T_t* array_T) {
    return array_T->values[array_T->size - 1];
}

void array_T_set_R(array_T_t* array_T, size_t index, T P value) {
    array_T->values[index] = P(value);
}

void array_T_push_back_R(array_T_t* array_T, T P value) {
    if(array_T_full(array_T)) {
        array_T_grow(array_T);
    }
    array_T->values[(array_T->size)++] = P(value);
}

void array_T_pop_back(array_T_t* array_T) {
    --(array_T->size);
}

void array_T_for_each_R(array_T_t * array_T, void(*func)(const T P)) {
    for(size_t i = 0; i < array_T->size; ++i) {
        func(A(array_T->values[i]));
    }
}

void array_T_grow(array_T_t* array_T) {
    size_t new_capacity = 2 * array_T->capacity;
    array_T->values = (T*)realloc(array_T->values, sizeof(T) * new_capacity);
    array_T->capacity = new_capacity;
}