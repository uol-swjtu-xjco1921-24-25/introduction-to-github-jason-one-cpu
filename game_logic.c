#include "game_logic.h"
#include <stdio.h>
#include <ctype.h>

/**
 * @brief Print maze with player position
 * @param maze Maze pointer
 * @param player Current player coordinates
 */
void print_maze(const Maze *maze, const Coord *player) {
    printf("\n");
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            printf("%c", (x == player->x && y == player->y) ? 'X' : maze->map[y][x]);
        }
        printf("\n");
    }
}

/**
 * @brief Process player movement
 * @param maze Maze pointer
 * @param player Player coordinates (modified in-place)
 * @param direction Input direction character
 */
void process_move(const Maze *maze, Coord *player, char direction) {
    static const MoveDelta deltas[] = {{'w', 0, -1}, {'a', -1, 0}, {'s', 0, 1}, {'d', 1, 0}};

    direction = tolower(direction);
    for (size_t i = 0; i < sizeof(deltas)/sizeof(deltas[0]); i++) {
        if (deltas[i].dir == direction) {
            Coord new_pos = {player->x + deltas[i].dx, player->y + deltas[i].dy};

            if (new_pos.x < 0 || new_pos.x >= maze->width || new_pos.y < 0 || new_pos.y >= maze->height) {
                printf("Out of bounds!\n");
                return;
            }

            if (maze->map[new_pos.y][new_pos.x] == '#') {
                printf("Wall detected!\n");
                return;
            }

            *player = new_pos;
            return;
        }
    }
    printf("Invalid command. Use WASD to move.\n");
}