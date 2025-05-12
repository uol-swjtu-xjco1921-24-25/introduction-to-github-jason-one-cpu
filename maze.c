/**
 * @file maze.c
 * @author (Zixuan Sun)
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

// defines for the required autograder exit codes
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3
#define EXIT_OTHER_ERROR 100

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 *
 * @param this pointer to the maze to be initialised
 * @param height height to allocate
 * @param width width to allocate
 * @return int 0 on success, 1 on fail
 */
int create_maze(maze *this, int height, int width)
{
    this->map = (char **)malloc(height * sizeof(char *));
    if (!this->map)
        return 1;

    for (int i = 0; i < height; i++)
    {
        this->map[i] = (char *)malloc(width * sizeof(char));
        if (!this->map[i])
        {
            for (int j = 0; j < i; j++)
                free(this->map[j]);
            free(this->map);
            return 1;
        }
    }
    this->height = height;
    this->width = width;
    return 0;
}

/**
 * @brief Free the memory allocated to the maze struct
 *
 * @param this the pointer to the struct to free
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
    }
}

/**
 * @brief Validate and return the width of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid width (5-100)
 */
int get_width(FILE *file)
{
    char line[MAX_DIM + 2];
    if (!fgets(line, sizeof(line), file))
        return 0;

    int width = strlen(line);
    if (line[width - 1] == '\n')
        width--;

    long pos = ftell(file);
    char verify_line[MAX_DIM + 2];
    while (fgets(verify_line, sizeof(verify_line), file))
    {
        int verify_len = strlen(verify_line);
        if (verify_len > 0 && verify_line[verify_len - 1] == '\n')
            verify_len--;
        if (verify_len != width)
        {
            fseek(file, pos, SEEK_SET);
            return 0;
        }
    }
    fseek(file, pos, SEEK_SET);

    if (width < MIN_DIM || width > MAX_DIM)
        return 0;
    return width;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file)
{
    int height = 0;
    char line[MAX_DIM + 2];

    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        int line_len = strlen(line);
        if (line_len > 0 && line[line_len - 1] == '\n')
            line_len--;
        if (line_len < MIN_DIM || line_len > MAX_DIM)
            return 0;
        height++;
    }
    return (height >= MIN_DIM && height <= MAX_DIM) ? height : 0;
}

/**
 * @brief read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on fail
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

        if (strlen(line) != this->width)
            return 1;

        for (int j = 0; j < this->width; j++)
        {
            char c = line[j];
            if (c == 'S')
            {
                this->start.x = j;
                this->start.y = i;
                s_count++;
            }
            else if (c == 'E')
            {
                this->end.x = j;
                this->end.y = i;
                e_count++;
            }
            else if (c != '#' && c != ' ')
            {
                return 1;
            }
            this->map[i][j] = c;
        }
    }
    return (s_count == 1 && e_count == 1) ? 0 : 1;
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this pointer to maze to print
 * @param player the current player location
 */
void print_maze(maze *this, coord *player)
{
    // make sure we have a leading newline..
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            // decide whether player is on this spot or not
            if (player->x == j && player->y == i)
            {
                printf("X");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        // end each row with a newline.
        printf("\n");
    }
}

/**
 * @brief Validates and performs a movement in a given direction
 *
 * @param this Maze struct
 * @param player The player's current position
 * @param direction The desired direction to move in
 */
void move(maze *this, coord *player, char direction)
{
    int dx = 0, dy = 0;
    direction = tolower(direction);

    switch (direction)
    {
    case 'w':
        dy = -1;
        break;
    case 'a':
        dx = -1;
        break;
    case 's':
        dy = 1;
        break;
    case 'd':
        dx = 1;
        break;
    default:
        printf("Invalid direction! Use WASD.\n");
        return;
    }

    int new_x = player->x + dx;
    int new_y = player->y + dy;

    if (new_x < 0 || new_x >= this->width ||
        new_y < 0 || new_y >= this->height)
    {
        printf("Invalid move!\n");
        return;
    }

    if (this->map[new_y][new_x] == '#')
    {
        printf("Wall in the way!\n");
        return;
    }

    player->x = new_x;
    player->y = new_y;
}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this current maze
 * @param player player position
 * @return int 0 for false, 1 for true
 */
int has_won(maze *this, coord *player)
{
    return (player->x == this->end.x && player->y == this->end.y);
}

int main(int argc, char *argv[])
{
    // check args
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <mazefile>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }
    // set up some useful variables (you can rename or remove these if you want)
    coord *player = malloc(sizeof(coord));
    maze *this_maze = malloc(sizeof(maze));
    if (!player || !this_maze)
    {
        if (player)
            free(player);
        if (this_maze)
            free(this_maze);
        return EXIT_OTHER_ERROR;
    }
    FILE *f = fopen(argv[1], "r");

    // open and validate mazefile
    if (!f)
    {
        perror("Error opening file");
        free(player);
        free(this_maze);
        return EXIT_FILE_ERROR;
    }
    // read in mazefile to struct
    int width = get_width(f);
    int height = get_height(f);
    if (!width || !height)
    {
        fclose(f);
        free(player);
        free(this_maze);
        return EXIT_MAZE_ERROR;
    }

    if (create_maze(this_maze, height, width))
    {
        fclose(f);
        free(player);
        free(this_maze);
        return EXIT_OTHER_ERROR;
    }

    if (read_maze(this_maze, f))
    {
        fclose(f);
        free_maze(this_maze);
        free(player);
        free(this_maze);
        return EXIT_MAZE_ERROR;
    }
    fclose(f);

    // maze game loop
    player->x = this_maze->start.x;
    player->y = this_maze->start.y;

    char input[256];
    while (1)
    {
        printf("Enter move (WASD/M/Q): ");
        if (!fgets(input, sizeof(input), stdin))
            break;

        char cmd = tolower(input[0]);
        if (cmd == 'm')
            print_maze(this_maze, player);
        else if (cmd == 'q')
            break;
        else if (strchr("wasd", cmd))
            move(this_maze, player, cmd);
        else
            printf("Invalid command! Valid options: W/A/S/D/M/Q\n");

        if (has_won(this_maze, player))
        {
            printf("Congratulations! You won!\n");
            break;
        }
    }
    // return, free, exit
    free_maze(this_maze);
    free(player);
    free(this_maze);
    return EXIT_SUCCESS;
}