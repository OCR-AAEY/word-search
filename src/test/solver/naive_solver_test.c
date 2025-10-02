#include <criterion/criterion.h>

#include "solver/grid.h"
#include "solver/naive_solver.h"

Test(naive_solver, naive_solve_1) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "HORIZONTAL", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 0);
    cr_expect_eq(s_w, 0);
    cr_expect_eq(e_h, 0);
    cr_expect_eq(e_w, 9);
}

Test(naive_solver, naive_solve_2) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "VERTICAL", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 7);
    cr_expect_eq(s_w, 9);
    cr_expect_eq(e_h, 0);
    cr_expect_eq(e_w, 9);
}

Test(naive_solver, naive_solve_3) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "DIAGONAL", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 1);
    cr_expect_eq(s_w, 0);
    cr_expect_eq(e_h, 8);
    cr_expect_eq(e_w, 7);
}

Test(naive_solver, naive_solve_4) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "FIND", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 8);
    cr_expect_eq(s_w, 4);
    cr_expect_eq(e_h, 5);
    cr_expect_eq(e_w, 1);
}

Test(naive_solver, naive_solve_5) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "HELLO", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 8);
    cr_expect_eq(s_w, 9);
    cr_expect_eq(e_h, 8);
    cr_expect_eq(e_w, 5);
}

Test(naive_solver, naive_solve_6) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "WORLD", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 5);
    cr_expect_eq(s_w, 5);
    cr_expect_eq(e_h, 5);
    cr_expect_eq(e_w, 1);
}

Test(naive_solver, naive_solve_7) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "GOLDORAK", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, 1);
    cr_expect_eq(s_w, 8);
    cr_expect_eq(e_h, 8);
    cr_expect_eq(e_w, 1);
}

Test(naive_solver, naive_solve_8) {
    Grid* grid = load_grid("./src/test/solver/grid_1.txt");
    int s_h, s_w, e_h, e_w;
    naive_solve(grid, "EPITA", &s_h, &s_w, &e_h, &e_w);
    free_grid(grid);

    cr_expect_eq(s_h, -1);
    cr_expect_eq(s_w, -1);
    cr_expect_eq(e_h, -1);
    cr_expect_eq(e_w, -1);
}
