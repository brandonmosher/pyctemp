#ifndef GRID_T_H
#define GRID_T_H

#include <unistd.h>
#include "coordinate.h"

#define T char
#define P *
#define A &

typedef T _T_t;

typedef struct {
	size_t num_rows;
	size_t num_cols;
    size_t num_cells;
	T* values;
} grid_T_t;

coordinate_t index_to_coordinate(
	const grid_T_t* grid_T,
	size_t grid_index);

size_t coordinate_to_index(
	const grid_T_t* grid_T,
	coordinate_t coordinate);

void grid_T_init(
	grid_T_t* grid_T,
	size_t num_rows,
	size_t num_cols,
	T* values);

void grid_T_copy(
	grid_T_t* grid_dest,
	const grid_T_t* grid_src);

void grid_T_destroy(grid_T_t* grid_T);

// void grid_T_resize(grid_T_t* grid_T, size_t num_rows, size_t num_cols);

size_t grid_T_count_value_R(
	const grid_T_t* grid_T,
	T P value,
	int(*comp)(const T P, const T P));

size_t grid_T_find_index_R(
	const grid_T_t* grid_T,
	T P value,
	size_t start,
	int(*comp)(const T P, const T P));

coordinate_t grid_T_find_coordinate_R(
	const grid_T_t* grid_T,
	T P value,
	coordinate_t start,
	int(*comp)(const T P, const T P));

T P grid_T_get_value_coordinate_R(
	const grid_T_t* grid_T,
	coordinate_t coordinate);

void grid_T_set_value_index_R(
	grid_T_t* grid_T,
	size_t index,
	T P value);

void grid_T_set_values_index_R(
	grid_T_t* grid_T,
	size_t begin_index,
	size_t end_index,
	T P value);

void grid_T_set_value_coordinate_R(
	grid_T_t* grid_T,
	coordinate_t coordinate,
	T P value);

void grid_T_set_values_coordinate_R(
	grid_T_t* grid_T,
	coordinate_t begin_coordinate,
	coordinate_t end_coordinate,
	T P value);

void grid_T_fill_R(
	grid_T_t* grid_T,
	T P value);

void grid_T_swap_value_index(
	grid_T_t* grid_T,
	size_t index_a,
	size_t index_b);

void grid_T_swap_value_coordinate(
	grid_T_t* grid_T,
	coordinate_t coordinate_a,
	coordinate_t coordinate_b);

int grid_T_coordinate_inbounds(
	const grid_T_t* grid_T,
	coordinate_t coordinate);

int grid_T_index_inbounds(
	const grid_T_t* grid_T,
	size_t index);

void grid_T_for_each_R(
	const grid_T_t* grid_T,
	void(*func)(const grid_T_t*, size_t, const T P));

#endif
