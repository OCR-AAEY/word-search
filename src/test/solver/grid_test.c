#include <criterion/criterion.h>

#include "solver/grid.h"

Test(solver, grid_load_from_file_test)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    cr_expect_arr_eq(grid_char_ptr(grid, 0, 0),
                     "HORIZONTALDXRAHCLBGADIKCILEOKCIGAJHYLYHIHGFGODTIOTGDLROWK"
                     "BFRPLNRDNERGEJHAIDUAJGVUKGFFOLLEH",
                     grid_height(grid) * grid_width(grid));
    grid_free(grid);
}

Test(solver, grid_load_from_file_failure_test_1, .exit_code = EXIT_FAILURE)
{
    Grid *grid = grid_load_from_file("./this_file_is_not_supposed_to_exist");
    grid_free(grid);
}

Test(solver, grid_load_from_file_failure_test_2, .exit_code = EXIT_FAILURE)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_2.txt");
    grid_free(grid);
}

Test(solver, grid_load_from_file_failure_test_3, .exit_code = EXIT_FAILURE)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_3.txt");
    grid_free(grid);
}

Test(solver, grid_solve_test_1)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "HORIZONTAL", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 0);
    cr_expect_eq(s_w, 0);
    cr_expect_eq(e_h, 0);
    cr_expect_eq(e_w, 9);
}

Test(solver, grid_solve_test_2)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "VERTICAL", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 7);
    cr_expect_eq(s_w, 9);
    cr_expect_eq(e_h, 0);
    cr_expect_eq(e_w, 9);
}

Test(solver, grid_solve_test_3)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "DIAGONAL", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 1);
    cr_expect_eq(s_w, 0);
    cr_expect_eq(e_h, 8);
    cr_expect_eq(e_w, 7);
}

Test(solver, grid_solve_test_4)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "FIND", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 8);
    cr_expect_eq(s_w, 4);
    cr_expect_eq(e_h, 5);
    cr_expect_eq(e_w, 1);
}

Test(solver, grid_solve_test_5)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "HELLO", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 8);
    cr_expect_eq(s_w, 9);
    cr_expect_eq(e_h, 8);
    cr_expect_eq(e_w, 5);
}

Test(solver, grid_solve_test_6)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "WORLD", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 5);
    cr_expect_eq(s_w, 5);
    cr_expect_eq(e_h, 5);
    cr_expect_eq(e_w, 1);
}

Test(solver, grid_solve_test_7)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "GOLDORAK", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, 1);
    cr_expect_eq(s_w, 8);
    cr_expect_eq(e_h, 8);
    cr_expect_eq(e_w, 1);
}

Test(solver, grid_solve_test_8)
{
    Grid *grid = grid_load_from_file("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    grid_solve_word(grid, "EPITA", &s_h, &s_w, &e_h, &e_w);
    grid_free(grid);

    cr_expect_eq(s_h, -1);
    cr_expect_eq(s_w, -1);
    cr_expect_eq(e_h, -1);
    cr_expect_eq(e_w, -1);
}
