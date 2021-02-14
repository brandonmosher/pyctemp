#ifndef LIST_T_H
#define LIST_T_H

#include <unistd.h>

#define T int
#define P *
#define A &
#define R r

typedef struct node_t node_t;

typedef struct {
	node_t * head;
	node_t * tail;
} list_T_t;

void list_T_init(list_T_t * list);

void list_T_destroy(list_T_t * list);

void list_T_clear(list_T_t * list);

size_t list_T_size(const list_T_t * list);

int list_T_empty(const list_T_t * list);

T * list_T_get(const list_T_t * list, size_t i);

T * list_T_front(const list_T_t * list);

T * list_T_back(const list_T_t * list);

T * list_T_insert_after_R(list_T_t * list, size_t i, const T P data);

T * list_T_insert_before_R(list_T_t * list, size_t i, const T P data);

T * list_T_push_back_R(list_T_t * list, const T P data);

T * list_T_push_front_R(list_T_t * list, const T P data);

void list_T_swap(list_T_t * list, size_t i, size_t j);

void list_T_for_each_R(list_T_t * list, void(*func)(const T P));

void list_T_pop_back(list_T_t * list);

void list_T_pop_front(list_T_t * list);

void list_T_remove(list_T_t * list, size_t i);

void list_T_bubble_sort_R(list_T_t * list, int(*comp)(const T P, const T P));

#endif