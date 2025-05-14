#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

// Dimension constraints
#define MAX_DIM 100
#define MIN_DIM 5

// Coordinate structure
typedef struct Coord
{
    int x;
    int y;
} Coord;

// Maze structure
typedef struct Maze
{
    char **map;
    int width;
    int height;
    Coord start;
    Coord end;
} Maze;

// Maze initialization and cleanup
bool maze_create(Maze *maze, int height, int width);
void maze_free(Maze *maze);

#endif