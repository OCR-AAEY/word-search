#include <criterion/criterion.h>

#include "solver/grid.h"


Test(solver, load_grid_1) {
    Grid grid = load_grid("./src/test/solver/grid_1.txt");
    cr_assert_eq(*grid.array, 'H', "First char should be H but got %c", *grid.array);
}
