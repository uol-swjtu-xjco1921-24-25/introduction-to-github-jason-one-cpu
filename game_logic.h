#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "maze.h"

// Movement delta structure
typedef struct MoveDelta
{
    char dir;
    int dx;
    int dy;
} MoveDelta;

// Game functions
void print_maze(const Maze *maze, const Coord *player);
void process_move(const Maze *maze, Coord *player, char direction);

#endif