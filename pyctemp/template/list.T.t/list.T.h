#ifndef LIST_T_H
#define LIST_T_H

#define T int
#define P *
#define A &

typedef T _T_t;

#include <unistd.h>

typedef struct node_t node_t;

typedef struct {
	node_t * head;
	node_t * tail;
} list_T_t;

void list_T_init(list_T_t * list_T);

void list_T_destroy(list_T_t * list_T);

void list_T_clear(list_T_t * list_T);

size_t list_T_size(const list_T_t * list_T);

int list_T_empty(const list_T_t * list_T);

T P list_T_get_R(const list_T_t * list_T, size_t index);

T P list_T_front_R(const list_T_t * list_T);

T P list_T_back_R(const list_T_t * list_T);

T * list_T_insert_after_R(list_T_t * list_T, size_t index, const T P value);

T * list_T_insert_before_R(list_T_t * list_T, size_t index, const T P value);

T * list_T_push_back_R(list_T_t * list_T, const T P value);

T * list_T_push_front_R(list_T_t * list_T, const T P value);

void list_T_swap(list_T_t * list_T, size_t index_a, size_t index_b);

void list_T_for_each_R(list_T_t * list_T,
	void(*func)(list_T_t * list_T, size_t index, const T P));

void list_T_pop_back(list_T_t * list_T);

void list_T_pop_front(list_T_t * list_T);

void list_T_remove(list_T_t * list_T, size_t index);

void list_T_bubble_sort_R(list_T_t * list_T,
	int(*comp)(const T P, const T P));

#endif