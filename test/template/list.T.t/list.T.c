#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "list.T.h"

struct node_t {
   node_t * prev;
   node_t * next;
   T data;
};

void list_T_init(list_T_t * list) {
	list->head = NULL;
	list->tail = NULL;
}

void list_T_destroy(list_T_t * list) {
   node_t * cur = list->head;
   node_t * next = cur;
   while(cur) {
       next = cur->next;
       free(cur);
       cur = next;
   }
   list->head = NULL;
   list->tail = NULL;
}

void list_T_clear(list_T_t * list) {
	list_T_destroy(list);
}

size_t list_T_size(const list_T_t * list) {
	size_t size = 0;
	node_t * node = list->head;
	while(node) {
		++size;
		node = node->next;
	}
	return size;
}

int list_T_empty(const list_T_t * list) {
	return (list->head == NULL);
}

static node_t * list_T_new_node() {
	node_t * node = (node_t *)malloc(sizeof(node_t));
	memset(node, 0, sizeof(node_t));
	return node;
}

static node_t * list_T_get_node(const list_T_t * list, size_t i) {
	node_t * node = list->head;
	int j = 0;
	for(; (j < i) && (node != NULL);  ++j) {
		node = node->next;
	}
	return node;
}

T * list_T_get(const list_T_t * list, size_t i) {
	node_t * node = list_T_get_node(list, i);
	return &(node->data);
}

T * list_T_front(const list_T_t * list) {
	return &(list->head->data);
}

T * list_T_back(const list_T_t * list) {
	return &(list->tail->data);
}

static T * list_T_insert_after_node_R(list_T_t * list, node_t * target, const T P data) {
	node_t * node = list_T_new_node();
	node->data = P(data);
	if(!target && list_T_empty(list)) { // size 0
		list->head = node;
		list->tail = list->head;
	}
	else if(list->head == list->tail) { //size 1
		node->prev = list->head;
		list->tail = node;
		list->head->next = node;
	}
	else if(target == list->tail) {
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
	}
	else {
		node->next = target->next;
		node->prev = target;
		target->next->prev = node;
		target->next = node;
	}
	return &(node->data);
}

static T * list_T_insert_before_node_R(list_T_t * list, node_t * target, const T P data) {
	return list_T_insert_after_node_R(list, target->prev, data);
}

T * list_T_insert_before_R(list_T_t * list, size_t i, const T P data) {
	return list_T_insert_before_node_R(list, list_T_get_node(list, i), data);
}

T * list_T_insert_after_R(list_T_t * list, size_t i, const T P data) {
	return list_T_insert_after_node_R(list, list_T_get_node(list, i), data);
}

T * list_T_push_front_R(list_T_t * list, const T P data) {
	return list_T_insert_before_node_R(list, list->head, data);
}

T * list_T_push_back_R(list_T_t * list, const T P data) {
	return list_T_insert_after_node_R(list, list->tail, data);
}

static void list_T_remove_node(list_T_t * list, node_t * node) {
	if(node == list->head && node == list->tail) {
		list->head = NULL;
		list->tail = NULL;
	}
	else if(node == list->head) {
		list->head = node->next;
		list->head->prev = NULL;
	}
	else if(node == list->tail) {
		list->tail = node->prev;
		list->tail->next = NULL;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	free(node);
}

void list_T_pop_front(list_T_t * list) {
	list_T_remove_node(list, list->head);
}

void list_T_pop_back(list_T_t * list) {
	list_T_remove_node(list, list->tail);
}

void list_T_remove(list_T_t * list, size_t i) {
	node_t * node = list_T_get_node(list, i);
	if(node) {
		list_T_remove_node(list, node);
	}
}

static void list_T_swap_nodes(node_t * a, node_t * b) {
	T tmp = b->data;
	b->data = a->data;
	a->data = tmp;
}

void list_T_swap(list_T_t * list, size_t i, size_t j) {
	list_T_swap_nodes(list_T_get_node(list, i), list_T_get_node(list, j));
}

void list_T_for_each_R(list_T_t * list, void(*func)(const T P)) {
	node_t * node = list->head;
	while(node) {
		func(A(node->data));
		node = node->next;
	}
}

void list_T_bubble_sort_R(list_T_t * list, int(*comp)(const T P, const T P)) {
	if(list->head == list->tail) {
		return;
	}
	node_t * start = list->head;
	node_t * cur = start->next;
	node_t * min = start;
	while(1) {
		while(cur) {
			if(comp(A(cur->data), A(min->data))) {
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