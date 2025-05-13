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
typedef struct __Coord
{
    int x;
    int y;
} coord;

// Maze structure
typedef struct __Maze
{
    char **map;
    int width;
    int height;
    coord start;
    coord end;
} maze;

// Movement delta structure
typedef struct __MoveDelta
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
    if (!*ptr)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    return true;
}

/**
 * @brief Unified error handler
 * @param message Error description
 */
static void handle_error(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_OTHER_ERROR);
}

/* ================== Core Functionality ================== */

/**
 * @brief Initialize maze memory structure
 * @param this Maze pointer
 * @param height Maze height
 * @param width Maze width
 * @return 0 on success, 1 on failure
 */
int create_maze(maze *this, int height, int width)
{
    // Validate dimensions
    if (height < MIN_DIM || height > MAX_DIM ||
        width < MIN_DIM || width > MAX_DIM)
    {
        return 1;
    }

    // Allocate row pointers
    if (!safe_malloc((void **)&this->map, height * sizeof(char *)))
    {
        return 1;
    }

    // Initialize structure
    memset(this->map, 0, height * sizeof(char *));
    this->width = width;
    this->height = height;
    this->start = (coord){-1, -1};
    this->end = (coord){-1, -1};

    // Allocate column space
    for (int i = 0; i < height; i++)
    {
        if (!safe_malloc((void **)&this->map[i], width * sizeof(char)))
        {
            // Rollback allocations
            for (int j = 0; j < i; j++)
                free(this->map[j]);
            free(this->map);
            return 1;
        }
        memset(this->map[i], '#', width);
    }
    return 0;
}

/**
 * @brief Free maze memory
 * @param this Maze pointer
 */
void free_maze(maze *this)
{
    if (this->map)
    {
        for (int i = 0; i < this->height; i++)
        {
            free(this->map[i]);
        }
        free(this->map);
        this->map = NULL;
    }
}

/* ================== File Handling ================== */

/**
 * @brief Validate maze file dimensions
 * @param file File pointer
 * @return Valid width (5-100) or 0 for error
 */
int get_width(FILE *file)
{
    char line[MAX_DIM + 2];
    if (!fgets(line, sizeof(line), file))
        return 0;

    int width = strcspn(line, "\n");
    if (width < MIN_DIM || width > MAX_DIM)
        return 0;

    // Verify consistent line widths
    long pos = ftell(file);
    char verify_line[MAX_DIM + 2];
    while (fgets(verify_line, sizeof(verify_line), file))
    {
        int verify_len = strcspn(verify_line, "\n");
        if (verify_len != width)
        {
            fseek(file, pos, SEEK_SET);
            return 0;
        }
    }
    fseek(file, pos, SEEK_SET);
    return width;
}

/**
 * @brief Validate maze file height
 * @param file File pointer
 * @return Valid height (5-100) or 0 for error
 */
int get_height(FILE *file)
{
    rewind(file);
    int height = 0;
    char line[MAX_DIM + 2];

    while (fgets(line, sizeof(line), file))
    {
        int line_len = strcspn(line, "\n");
        if (line_len < MIN_DIM || line_len > MAX_DIM)
            return 0;
        height++;
    }
    return (height >= MIN_DIM && height <= MAX_DIM) ? height : 0;
}

/**
 * @brief Load maze file into structure
 * @param this Maze structure
 * @param file File pointer
 * @return 0 on success, 1 on failure
 */
int read_maze(maze *this, FILE *file)
{
    rewind(file);
    int s_count = 0, e_count = 0;
    char line[MAX_DIM + 2];

    for (int i = 0; i < this->height; i++)
    {
        if (!fgets(line, sizeof(line), file))
            return 1;
        line[strcspn(line, "\n")] = '\0';

        for (int j = 0; j < this->width; j++)
        {
            char c = line[j];
            switch (c)
            {
            case 'S':
                this->start.x = j;
                this->start.y = i;
                s_count++;
                break;
            case 'E':
                this->end.x = j;
                this->end.y = i;
                e_count++;
                break;
            case ' ':
            case '#':
                break;
            default:
                return 1;
            }
            this->map[i][j] = c;
        }
    }
    return (s_count == 1 && e_count == 1) ? 0 : 1;
}

/* ================== Game Logic ================== */

/**
 * @brief Print maze with player position
 * @param this Maze structure
 * @param player Current player coordinates
 */
void print_maze(maze *this, coord *player)
{
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            printf("%c", (player->x == j && player->y == i) ? 'X' : this->map[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Process player movement
 * @param this Maze structure
 * @param player Player coordinates
 * @param direction Movement direction
 */
void move(maze *this, coord *player, char direction)
{
    static const MoveDelta directions[] = {
        {'w', 0, -1}, {'a', -1, 0}, {'s', 0, 1}, {'d', 1, 0}};

    direction = tolower(direction);
    for (size_t i = 0; i < sizeof(directions) / sizeof(directions[0]); i++)
    {
        if (directions[i].dir == direction)
        {
            coord new_pos = {
                player->x + directions[i].dx,
                player->y + directions[i].dy};

            // Boundary check
            if (new_pos.x < 0 || new_pos.x >= this->width ||
                new_pos.y < 0 || new_pos.y >= this->height)
            {
                printf("Invalid move: out of bounds\n");
                return;
            }

            // Collision detection
            if (this->map[new_pos.y][new_pos.x] == '#')
            {
                printf("Invalid move: wall detected\n");
                return;
            }

            *player = new_pos;
            return;
        }
    }
    printf("Invalid command: use WASD to move\n");
}

/**
 * @brief Check victory condition
 * @param this Maze structure
 * @param player Player coordinates
 * @return 1 if player reached exit, 0 otherwise
 */
int has_won(maze *this, coord *player)
{
    return (player->x == this->end.x && player->y == this->end.y);
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

    // File existence check
    FILE *f = fopen(argv[1], "r");
    if (!f)
        handle_error("File not found");

    // Initialize maze structure
    maze *this_maze = malloc(sizeof(maze));
    coord *player = malloc(sizeof(coord));
    if (!this_maze || !player)
        handle_error("Memory allocation failed");

    // Read maze dimensions
    int width = get_width(f);
    int height = get_height(f);
    if (!width || !height || create_maze(this_maze, height, width))
    {
        fclose(f);
        handle_error("Invalid maze dimensions");
    }

    // Load maze content
    rewind(f);
    if (read_maze(this_maze, f))
    {
        fclose(f);
        handle_error("Invalid maze content");
    }
    fclose(f);

    // Initialize player position
    *player = this_maze->start;

    // Game loop
    char input;
    while (1)
    {
        printf("Enter move (WASD/M/Q): ");
        scanf(" %c", &input);

        if (tolower(input) == 'm')
        {
            print_maze(this_maze, player);
        }
        else if (tolower(input) == 'q')
        {
            break;
        }
        else
        {
            move(this_maze, player, input);
        }

        if (has_won(this_maze, player))
        {
            printf("\n=== Victory! ===\n");
            break;
        }
    }

    // Cleanup
    free_maze(this_maze);
    free(this_maze);
    free(player);
    return EXIT_SUCCESS;
}