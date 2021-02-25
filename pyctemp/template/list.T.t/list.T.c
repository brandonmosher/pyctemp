#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.T.h"

struct node_t {
   node_t * prev;
   node_t * next;
   T value;
};

void list_T_init(list_T_t * list_T) {
	list_T->head = NULL;
	list_T->tail = NULL;
}

void list_T_destroy(list_T_t * list_T) {
   node_t * cur = list_T->head;
   node_t * next = cur;
   while(cur) {
       next = cur->next;
       free(cur);
       cur = next;
   }
   list_T->head = NULL;
   list_T->tail = NULL;
}

void list_T_clear(list_T_t * list_T) {
	list_T_destroy(list_T);
}

size_t list_T_size(const list_T_t * list_T) {
	size_t size = 0;
	node_t * node = list_T->head;
	while(node) {
		++size;
		node = node->next;
	}
	return size;
}

int list_T_empty(const list_T_t * list_T) {
	return (list_T->head == NULL);
}

static node_t * list_T_new_node() {
	node_t * node = (node_t *)calloc(1, sizeof(node_t));
	return node;
}

static node_t * list_T_get_node(const list_T_t * list_T, size_t index) {
	node_t * node = list_T->head;
	for(size_t j; (j < index) && (node != NULL);  ++j) {
		node = node->next;
	}
	return node;
}

T P list_T_get_R(const list_T_t * list_T, size_t index) {
	node_t * node = list_T_get_node(list_T, index);
	return A(node->value);
}

T P list_T_front_R(const list_T_t * list_T) {
	return A(list_T->head->value);
}

T P list_T_back_R(const list_T_t * list_T) {
	return A(list_T->tail->value);
}

static T * list_T_insert_after_node_R(list_T_t * list_T, node_t * target, const T P value) {
	node_t * node = list_T_new_node();
	node->value = P(value);
	if(!target && list_T_empty(list_T)) { // size 0
		list_T->head = node;
		list_T->tail = list_T->head;
	}
	else if(list_T->head == list_T->tail) { //size 1
		node->prev = list_T->head;
		list_T->tail = node;
		list_T->head->next = node;
	}
	else if(target == list_T->tail) {
		node->prev = list_T->tail;
		list_T->tail->next = node;
		list_T->tail = node;
	}
	else {
		node->next = target->next;
		node->prev = target;
		target->next->prev = node;
		target->next = node;
	}
	return &(node->value);
}

static T * list_T_insert_before_node_R(list_T_t * list_T, node_t * target, const T P value) {
	return list_T_insert_after_node_R(list_T, target->prev, value);
}

T * list_T_insert_before_R(list_T_t * list_T, size_t index, const T P value) {
	return list_T_insert_before_node_R(list_T, list_T_get_node(list_T, index), value);
}

T * list_T_insert_after_R(list_T_t * list_T, size_t index, const T P value) {
	return list_T_insert_after_node_R(list_T, list_T_get_node(list_T, index), value);
}

T * list_T_push_front_R(list_T_t * list_T, const T P value) {
	return list_T_insert_before_node_R(list_T, list_T->head, value);
}

T * list_T_push_back_R(list_T_t * list_T, const T P value) {
	return list_T_insert_after_node_R(list_T, list_T->tail, value);
}

static void list_T_remove_node(list_T_t * list_T, node_t * node) {
	if(node == list_T->head && node == list_T->tail) {
		list_T->head = NULL;
		list_T->tail = NULL;
	}
	else if(node == list_T->head) {
		list_T->head = node->next;
		list_T->head->prev = NULL;
	}
	else if(node == list_T->tail) {
		list_T->tail = node->prev;
		list_T->tail->next = NULL;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	free(node);
}

void list_T_pop_front(list_T_t * list_T) {
	list_T_remove_node(list_T, list_T->head);
}

void list_T_pop_back(list_T_t * list_T) {
	list_T_remove_node(list_T, list_T->tail);
}

void list_T_remove(list_T_t * list_T, size_t index) {
	node_t * node = list_T_get_node(list_T, index);
	if(node) {
		list_T_remove_node(list_T, node);
	}
}

static void list_T_swap_nodes(node_t * a, node_t * b) {
	T tmp = b->value;
	b->value = a->value;
	a->value = tmp;
}

void list_T_swap(list_T_t * list_T, size_t index_a, size_t index_b) {
	list_T_swap_nodes(list_T_get_node(list_T, index_a), list_T_get_node(list_T, index_b));
}

void list_T_for_each_R(list_T_t * list_T, void(*func)(const T P)) {
	node_t * node = list_T->head;
	while(node) {
		func(A(node->value));
		node = node->next;
	}
}

void list_T_bubble_sort_R(list_T_t * list_T, int(*comp)(const T P, const T P)) {
	if(list_T->head == list_T->tail) {
		return;
	}
	node_t * start = list_T->head;
	node_t * cur = start->next;
	node_t * min = start;
	while(1) {
		while(cur) {
			if(comp(A(cur->value), A(min->value))) {
				min = cur;
			}
			cur = cur->next;
		}
		list_T_swap_nodes(start, min);
		if(!start->next) {
			break;
		}
		start = start->next;
		cur = start->next;
		min = start;
	}
}