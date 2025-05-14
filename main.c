#include <stdio.h>
#include <ctype.h>
#include "maze.h"
#include "file_io.h"
#include "game_logic.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <mazefile>\n", argv[0]);
        return 1;
    }

    Maze maze = {0};
    if (!maze_load(&maze, argv[1]))
    {
        maze_free(&maze);
        handle_error("Invalid maze", 3);
    }

    Coord player = maze.start;
    char input[256];
    while (1)
    {
        printf("\nCurrent position: (%d, %d)\n", player.x, player.y);
        printf("Enter command (WASD/M/Q): ");
        if (!fgets(input, sizeof(input), stdin))
            break;

        char cmd = tolower(input[0]);
        switch (cmd)
        {
        case 'm':
            print_maze(&maze, &player);
            break;
        case 'q':
            goto cleanup;
        default:
            process_move(&maze, &player, cmd);
        }

        if (player.x == maze.end.x && player.y == maze.end.y)
        {
            printf("\n=== You won! ===\n");
            break;
        }
    }

cleanup:
    maze_free(&maze);
    return 0;
}