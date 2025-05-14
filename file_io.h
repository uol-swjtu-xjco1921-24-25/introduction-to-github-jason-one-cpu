#ifndef FILE_IO_H
#define FILE_IO_H

#include "maze.h"
#include <stdio.h>

// File validation and loading
bool validate_maze_file(FILE *file, int *width, int *height);
bool maze_load(Maze *maze, const char *filename);

#endif