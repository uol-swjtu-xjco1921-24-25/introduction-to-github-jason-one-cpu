#include "maze.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initialize maze memory structure
 * @param maze Maze pointer
 * @param height Maze height
 * @param width Maze width
 * @return true on success, false on failure
 */
bool maze_create(Maze *maze, int height, int width)
{
    if (height < MIN_DIM || height > MAX_DIM || width < MIN_DIM || width > MAX_DIM)
    {
        return false;
    }

    maze->map = malloc(height * sizeof(char *));
    if (!maze->map)
        return false;

    memset(maze->map, 0, height * sizeof(char *));
    maze->width = width;
    maze->height = height;
    maze->start = (Coord){-1, -1};
    maze->end = (Coord){-1, -1};

    for (int i = 0; i < height; i++)
    {
        maze->map[i] = malloc(width * sizeof(char));
        if (!maze->map[i])
        {
            for (int j = 0; j < i; j++)
                free(maze->map[j]);
            free(maze->map);
            return false;
        }
        memset(maze->map[i], '#', width);
    }
    return true;
}

/**
 * @brief Free maze memory
 * @param maze Maze pointer
 */
void maze_free(Maze *maze)
{
    if (maze->map)
    {
        for (int i = 0; i < maze->height; i++)
            free(maze->map[i]);
        free(maze->map);
    }
}