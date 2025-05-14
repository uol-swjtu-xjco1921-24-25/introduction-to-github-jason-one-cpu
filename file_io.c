#include "file_io.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Validate maze file dimensions and content
 * @param file File pointer
 * @param width Output width
 * @param height Output height
 * @return true if valid, false otherwise
 */
bool validate_maze_file(FILE *file, int *width, int *height) {
    char buffer[MAX_DIM + 2];
    *height = 0;

    if (!fgets(buffer, sizeof(buffer), file)) return false;
    *width = strcspn(buffer, "\n");
    if (*width < MIN_DIM || *width > MAX_DIM) return false;

    do {
        (*height)++;
        int current_width = strcspn(buffer, "\n");
        if (current_width != *width) return false;
    } while (fgets(buffer, sizeof(buffer), file));

    return (*height >= MIN_DIM && *height <= MAX_DIM);
}

/**
 * @brief Load maze file into structure
 * @param maze Maze pointer
 * @param filename Maze file path
 * @return true on success, false on invalid maze
 */
bool maze_load(Maze *maze, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) handle_error("File not found", 2);

    int width, height;
    if (!validate_maze_file(file, &width, &height)) {
        fclose(file);
        return false;
    }

    rewind(file);
    if (!maze_create(maze, height, width)) {
        fclose(file);
        return false;
    }

    int start_count = 0, end_count = 0;
    char line[MAX_DIM + 2];
    for (int y = 0; y < height; y++) {
        fgets(line, sizeof(line), file);
        line[strcspn(line, "\n")] = '\0';

        for (int x = 0; x < width; x++) {
            char c = line[x];
            switch (c) {
                case 'S':
                    maze->start = (Coord){x, y};
                    start_count++;
                    break;
                case 'E':
                    maze->end = (Coord){x, y};
                    end_count++;
                    break;
                case ' ':
                case '#':
                    break;
                default:
                    fclose(file);
                    return false;
            }
            maze->map[y][x] = c;
        }
    }
    fclose(file);
    return (start_count == 1 && end_count == 1);
}