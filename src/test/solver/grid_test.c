#include <criterion/criterion.h>

#include "solver/grid.h"

Test(grid, load_grid_1) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    cr_assert_arr_eq(grid_content(grid),
                     "HORIZONTALDXRAHCLBGADIKCILEOKCIGAJHYLYHIHGFGODTIOTGDLROWK"
                     "BFRPLNRDNERGEJHAIDUAJGVUKGFFOLLEH",
                     grid_height(grid) * grid_width(grid),
                     "grid content differs from grid_1.txt");
    free_grid(grid);
}

Test(grid, load_grid_2, .exit_code = 1) {
    Grid * grid = load_grid("./this_file_is_not_supposed_to_exist");
    (void)grid;
}

Test(grid, load_grid_3, .exit_code = 1) {
    Grid * grid = load_grid("./src/test/solver/grid_2.txt");
    (void)grid;
}

Test(grid, load_grid_4, .exit_code = 1) {
    Grid * grid = load_grid("./src/test/solver/grid_3.txt");
    (void)grid;
}
