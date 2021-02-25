#include "grid.T.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

coordinate_t index_to_coordinate(
	const grid_T_t* grid_T,
	size_t grid_index)
{
	return make_coordinate(grid_index / grid_T->num_cols, grid_index % grid_T->num_cols);
}

size_t coordinate_to_index(
	const grid_T_t* grid_T,
	coordinate_t coordinate)
{
	return (coordinate.row * grid_T->num_cols) + coordinate.col;
}

void grid_T_init(
	grid_T_t* grid_T,
	size_t num_rows,
	size_t num_cols,
	T* values)
{
	grid_T->num_rows = num_rows;
	grid_T->num_cols = num_cols;
	grid_T->num_cells = num_rows * num_cols;
	if(values) {
		grid_T->values = (T*)malloc(grid_T->num_cells * sizeof(T));
		memcpy(grid_T->values, values, grid_T->num_cells * sizeof(T));
	}
	else {
		grid_T->values = (T*)calloc(grid_T->num_cells, sizeof(T));
	}
}

void grid_T_copy(
	grid_T_t* grid_dest,
	const grid_T_t* grid_src)
{
	grid_dest->num_rows = grid_src->num_rows;
	grid_dest->num_cols = grid_src->num_cols;
	memcpy(grid_dest->values, grid_src->values, grid_src->num_cells);
}

void grid_T_destroy(grid_T_t* grid_T) {
	if(grid_T->values) {
		free(grid_T->values);
		grid_T->values = NULL;
	}
}

// void grid_T_resize(grid_T_t* grid_T, size_t num_rows, size_t num_cols) {

// }

size_t grid_T_count_value_R(
	const grid_T_t* grid_T,
	T P value,
	int(*comp)(const T P, const T P))
{
	size_t count = 0;
	size_t start = 0;
	while(1) {
		start = grid_T_find_index_R(grid_T, value, start, comp);
		if(grid_T_index_inbounds(grid_T, start)) {
			++count;
			++start;
		}
		else {
			return count;
		}
	}
}

size_t grid_T_find_index_R(
	const grid_T_t* grid_T,
	T P value,
	size_t start,
	int(*comp)(const T P, const T P))
{
	for(size_t i = start; i < grid_T->num_cells; ++i) {
		if(comp) {
			if(comp(A(grid_T->values[i]), value)) {
				return i;
			}
		}
		else if(grid_T->values[i] == P(value)) {
			return i;
		}
	}
	return grid_T->num_cells;
}

coordinate_t grid_T_find_coordinate_R(
	const grid_T_t* grid_T,
	T P value,
	coordinate_t start,
	int(*comp)(const T P, const T P))
{
	size_t grid_index = grid_T_find_index_R(grid_T, value, coordinate_to_index(grid_T, start), comp);
	return index_to_coordinate(grid_T, grid_index);
}

T P grid_T_get_value_coordinate_R(
	const grid_T_t* grid_T,
	coordinate_t coordinate)
{
	return A(grid_T->values[coordinate_to_index(grid_T, coordinate)]);
}

void grid_T_set_value_index_R(
	grid_T_t* grid_T,
	size_t index,
	T P value)
{
	grid_T->values[index] = P(value);
}

void grid_T_set_values_index_R(
	grid_T_t* grid_T,
	size_t begin_index,
	size_t end_index,
	T P value)
{
	for(size_t index = begin_index; index < end_index; ++index) {
		grid_T_set_value_index_R(grid_T, index, value);
	}
}

void grid_T_set_value_coordinate_R(
	grid_T_t* grid_T,
	coordinate_t coordinate,
	T P value)
{
	grid_T_set_value_index_R(grid_T, coordinate_to_index(grid_T, coordinate), value);
}

void grid_T_set_values_coordinate_R(
	grid_T_t* grid_T,
	coordinate_t begin_coordinate,
	coordinate_t end_coordinate,
	T P value)
{
	grid_T_set_values_index_R(
		grid_T,
		coordinate_to_index(grid_T, begin_coordinate),
		coordinate_to_index(grid_T, end_coordinate),
		value);
}

void grid_T_fill_R(
	grid_T_t* grid_T,
	T P value)
{
	grid_T_set_values_index_R(grid_T, 0, grid_T->num_cells, value);
}

void grid_T_swap_value_index(
	grid_T_t* grid_T,
	size_t index_a,
	size_t index_b)
{
	T tmp = grid_T->values[index_a];
	grid_T->values[index_a] = grid_T->values[index_b];
	grid_T->values[index_b] = tmp;
}

void grid_T_swap_value_coordinate(
	grid_T_t* grid_T,
	coordinate_t coordinate_a,
	coordinate_t coordinate_b)
{
	grid_T_swap_value_index(
		grid_T,
		coordinate_to_index(grid_T, coordinate_a),
		coordinate_to_index(grid_T, coordinate_b)
	);
}

int grid_T_coordinate_inbounds(
	const grid_T_t* grid_T,
	coordinate_t coordinate)
{
    return (coordinate.row < grid_T->num_rows && coordinate.col < grid_T->num_cols);
}

int grid_T_index_inbounds(
	const grid_T_t* grid_T,
	size_t index)
{
    return index < grid_T->num_cells;
}

void grid_T_for_each_R(
	const grid_T_t* grid_T,
	void(*func)(const grid_T_t*, size_t, const T P))
{
	for(size_t i = 0; i < grid_T->num_cells; ++i) {
		func(grid_T, i, A(grid_T->values[i]));
	}
}