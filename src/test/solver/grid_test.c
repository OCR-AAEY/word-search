#include <criterion/criterion.h>

#include "solver/grid.h"

Test(solver, load_grid_1) {
    Grid grid = load_grid("./src/test/solver/grid_1.txt");
    cr_assert_arr_eq(grid.array,
                     "HORIZONTALDXRAHCLBGADIKCILEOKCIGAJHYLYHIHGFGODTIOTGDLROWK"
                     "BFRPLNRDNERGEJHAIDUAJGVUKGFFOLLEH",
                     grid.height * grid.width,
                     "grid content differs from grid_1.txt");
}

Test(solver, load_grid_2, .exit_code = 1) {
    Grid grid = load_grid("./this_file_is_not_supposed_to_exist");
    (void)grid;
}
