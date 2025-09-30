#ifndef GRID_H
#define GRID_H

typedef struct Grid Grid;

Grid* load_grid(char *file_name);

char* grid_content(Grid* grid);

size_t grid_height(Grid *grid);

size_t grid_width(Grid *grid);

char get_char(Grid *grid, size_t height, size_t width);

void free_grid(Grid *grid);

void print_grid(Grid *grid);

#endif
