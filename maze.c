/**
 * @file maze.c
 * @author Zixuan Sun
 * @brief Maze game implementation for COMP1921 Assignment 2
 * @note Follows defensive programming principles with strict error handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Dimension constraints
#define MAX_DIM 100
#define MIN_DIM 5

// Autograder exit codes
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3
#define EXIT_OTHER_ERROR 100

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

// Movement delta structure
typedef struct MoveDelta
{
    char dir;
    int dx;
    int dy;
} MoveDelta;

/* ================== Utility Functions ================== */

/**
 * @brief Safely allocate memory with error checking
 * @param ptr Double pointer to receive allocated memory
 * @param size Allocation size in bytes
 * @return true if successful, false otherwise
 */
static bool safe_malloc(void **ptr, size_t size)
{
    *ptr = malloc(size);
    return *ptr != NULL;
}

/**
 * @brief Unified error handler with exit codes
 * @param message Error description
 * @param code Exit code from specification
 */
static void handle_error(const char *message, int code)
{
    fprintf(stderr, "Error: %s\n", message);
    exit(code);
}

/* ================== Core Functionality ================== */

/**
 * @brief Initialize maze memory structure
 * @param maze Maze pointer
 * @param height Maze height
 * @param width Maze width
 * @return true on success, false on failure
 */
bool maze_create(Maze *maze, int height, int width)
{
    // Validate dimensions
    if (height < MIN_DIM || height > MAX_DIM || width < MIN_DIM || width > MAX_DIM)
    {
        return false;
    }

    // Allocate row pointers
    if (!safe_malloc((void **)&maze->map, height * sizeof(char *)))
    {
        return false;
    }

    // Initialize structure
    memset(maze->map, 0, height * sizeof(char *));
    maze->width = width;
    maze->height = height;
    maze->start = (Coord){-1, -1};
    maze->end = (Coord){-1, -1};

    // Allocate column space
    for (int i = 0; i < height; i++)
    {
        if (!safe_malloc((void **)&maze->map[i], width * sizeof(char)))
        {
            // Rollback allocations on failure
            for (int j = 0; j < i; j++)
                free(maze->map[j]);
            free(maze->map);
            return false;
        }
        memset(maze->map[i], '#', width); // Initialize as walls
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

/* ================== File Handling ================== */

/**
 * @brief Validate maze file dimensions and content
 * @param file File pointer
 * @param width Output width
 * @param height Output height
 * @return true if valid, false otherwise
 */
bool validate_maze_file(FILE *file, int *width, int *height)
{
    char buffer[MAX_DIM + 2];
    *height = 0;

    // Read first line to determine width
    if (!fgets(buffer, sizeof(buffer), file))
        return false; // 修正此处
    *width = strcspn(buffer, "\n");
    if (*width < MIN_DIM || *width > MAX_DIM)
        return false;

    // Verify all lines have consistent width
    do
    {
        (*height)++;
        int current_width = strcspn(buffer, "\n");
        if (current_width != *width)
            return false;
    } while (fgets(buffer, sizeof(buffer), file));

    return (*height >= MIN_DIM && *height <= MAX_DIM);
}

/**
 * @brief Load maze file into structure
 * @param maze Maze pointer
 * @param filename Maze file path
 * @return true on success, false on invalid maze
 */
bool maze_load(Maze *maze, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        handle_error("File not found", EXIT_FILE_ERROR);

    int width, height;
    if (!validate_maze_file(file, &width, &height))
    {
        fclose(file);
        return false;
    }

    rewind(file);
    if (!maze_create(maze, height, width))
    {
        fclose(file);
        return false;
    }

    int start_count = 0, end_count = 0;
    char line[MAX_DIM + 2];
    for (int y = 0; y < height; y++)
    {
        fgets(line, sizeof(line), file);
        line[strcspn(line, "\n")] = '\0'; // Trim newline

        for (int x = 0; x < width; x++)
        {
            char c = line[x];
            switch (c)
            {
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

/* ================== Game Logic ================== */

/**
 * @brief Print maze with player position
 * @param maze Maze pointer
 * @param player Current player coordinates
 */
void print_maze(const Maze *maze, const Coord *player)
{
    printf("\n");
    for (int y = 0; y < maze->height; y++)
    {
        for (int x = 0; x < maze->width; x++)
        {
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
void process_move(const Maze *maze, Coord *player, char direction)
{
    static const MoveDelta deltas[] = {
        {'w', 0, -1}, {'a', -1, 0}, {'s', 0, 1}, {'d', 1, 0}};

    direction = tolower(direction);
    for (size_t i = 0; i < sizeof(deltas) / sizeof(deltas[0]); i++)
    {
        if (deltas[i].dir == direction)
        {
            Coord new_pos = {
                player->x + deltas[i].dx,
                player->y + deltas[i].dy};

            // Boundary check
            if (new_pos.x < 0 || new_pos.x >= maze->width ||
                new_pos.y < 0 || new_pos.y >= maze->height)
            {
                printf("Out of bounds!\n");
                return;
            }

            // Collision check
            if (maze->map[new_pos.y][new_pos.x] == '#')
            {
                printf("Wall detected!\n");
                return;
            }

            *player = new_pos;
            return;
        }
    }
    printf("Invalid command. Use WASD to move.\n");
}

/* ================== Main Program ================== */

int main(int argc, char *argv[])
{
    // Argument validation
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <mazefile>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    // Load maze
    Maze maze = {0};
    if (!maze_load(&maze, argv[1]))
    {
        maze_free(&maze);
        handle_error("Invalid maze", EXIT_MAZE_ERROR);
    }

    // Initialize player
    Coord player = maze.start;
    char input[256];

    // Game loop
    while (true)
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

        // Victory check
        if (player.x == maze.end.x && player.y == maze.end.y)
        {
            printf("\n=== You won! ===\n");
            break;
        }
    }

cleanup:
    maze_free(&maze);
    return EXIT_SUCCESS;
}