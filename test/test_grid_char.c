#include "grid.int.h"
#include <stdio.h>

void grid_int_print(const grid_int_t* grid, size_t index, const int value) {
    printf("%i", value);
    if((index + 1) % grid->num_cols == 0) {
        printf("\n");
    }
}

int main() {
    grid_int_t grid;
    grid_int_init(&grid, 5, 5, NULL);
    grid_int_fill(&grid, 0);
    grid_int_for_each(&grid, grid_int_print);
    printf("\n");

    grid_int_set_values_coordinate(
        &grid,
        (coordinate_t){2,0},
        (coordinate_t){3,3},
        1
    );
    grid_int_for_each(&grid, grid_int_print);
    printf("\n");

    size_t num_zeros = grid_int_count_value(&grid, 0, NULL);
    size_t num_ones = grid_int_count_value(&grid, 1, NULL);

    printf("Counted %lu zeros and %lu ones\n", num_zeros, num_ones);
    
}