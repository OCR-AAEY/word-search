#ifndef NAIVE_H
#define NAIVE_H

int naive_solve(Grid *grid, char *word, int *start_height, int *start_width,
          int *end_height, int *end_width);

int check_horizontal(Grid *grid, char *word, int height, int width);

int check_vertical(Grid *grid, char *word, int height, int width);

int check_diagonal(Grid *grid, char *word, int height, int width);

#endif
