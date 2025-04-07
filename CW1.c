#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the maze structure
typedef struct
{
    char grid[100][100];  // 2D array to store maze layout
    int width;            // Width of the maze (columns)
    int height;           // Height of the maze (rows)
    int playerX;          // Player's current X position (column index)
    int playerY;          // Player's current Y position (row index)
    int exitX;            // X position of exit point 'E'
    int exitY;            // Y position of exit point 'E'
} Maze;

// Function declarations
Maze load_maze(const char *filename);
void display_map(const Maze *maze);
int handle_move(Maze *maze, char input);
void run_game_loop(Maze *maze);
int validate_maze(Maze *maze);

// Main function
int main(int argc, char *argv[])
{
    // Check command line parameters
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <maze_file>\n", argv[0]);
        return 1;
    }

    // Load maze from file
    Maze maze = load_maze(argv[1]);
    
    // Validate maze structure
    if (!validate_maze(&maze))
    {
        fprintf(stderr, "Invalid maze configuration\n");
        return 1;
    }

    // Start game loop
    run_game_loop(&maze);

    return 0;
}

// Load maze from file and initialize structure
Maze load_maze(const char *filename)
{
    FILE *file = fopen(filename, "r");
    Maze maze;
    memset(&maze, 0, sizeof(Maze));  // Initialize all fields to 0

    if (!file)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    // Read maze line by line
    int row = 0;
    while (fgets(maze.grid[row], sizeof(maze.grid[row]), file) && row < 100)
    {
        // Remove newline character
        size_t len = strlen(maze.grid[row]);
        if (len > 0 && maze.grid[row][len-1] == '\n')
        {
            maze.grid[row][len-1] = '\0';
        }

        // Set width based on first row
        if (row == 0)
        {
            maze.width = strlen(maze.grid[row]);
        }
        
        row++;
    }
    maze.height = row;

    fclose(file);
    return maze;
}

// Validate maze configuration
int validate_maze(Maze *maze)
{
    int start_count = 0, exit_count = 0;
    
    // Check maze dimensions
    if (maze->width < 5 || maze->width > 100 || 
        maze->height < 5 || maze->height > 100)
    {
        fprintf(stderr, "Invalid maze dimensions\n");
        return 0;
    }

    // Check rectangular shape and count start/exit points
    for (int y = 0; y < maze->height; y++)
    {
        if (strlen(maze->grid[y]) != (size_t)maze->width)
        {
            fprintf(stderr, "Maze is not rectangular\n");
            return 0;
        }

        for (int x = 0; x < maze->width; x++)
        {
            if (maze->grid[y][x] == 'S')
            {
                maze->playerX = x;
                maze->playerY = y;
                start_count++;
            }
            else if (maze->grid[y][x] == 'E')
            {
                maze->exitX = x;
                maze->exitY = y;
                exit_count++;
            }
        }
    }

    // Validate start/exit points
    if (start_count != 1 || exit_count != 1)
    {
        fprintf(stderr, "Maze must contain exactly one 'S' and one 'E'\n");
        return 0;
    }

    return 1;
}

// Display map with player position
void display_map(const Maze *maze)
{
    printf("\n");
    for (int y = 0; y < maze->height; y++)
    {
        for (int x = 0; x < maze->width; x++)
        {
            if (x == maze->playerX && y == maze->playerY)
            {
                printf("X");
            }
            else
            {
                printf("%c", maze->grid[y][x]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Handle player movement
int handle_move(Maze *maze, char input)
{
    int dx = 0, dy = 0;
    input = tolower(input);

    // Determine movement direction
    switch (input)
    {
        case 'w': dy = -1; break;
        case 's': dy = 1;  break;
        case 'a': dx = -1; break;
        case 'd': dx = 1;  break;
        default:  return 0;  // Invalid input
    }

    // Calculate new position
    int newX = maze->playerX + dx;
    int newY = maze->playerY + dy;

    // Check boundaries
    if (newX < 0 || newX >= maze->width || newY < 0 || newY >= maze->height)
    {
        printf("Cannot move outside maze boundaries!\n");
        return 0;
    }

    // Check for walls
    if (maze->grid[newY][newX] == '#')
    {
        printf("Cannot move through walls!\n");
        return 0;
    }

    // Update player position
    maze->playerX = newX;
    maze->playerY = newY;
    return 1;
}

// Main game loop
void run_game_loop(Maze *maze)
{
    char input;
    int game_running = 1;

    printf("=== Maze Game Started ===\n");
    
    while (game_running)
    {
        printf("Enter command (WASD to move, M to view map, Q to quit): ");
        scanf(" %c", &input);  // Space before %c to skip whitespace

        input = tolower(input);
        
        switch (input)
        {
            case 'w':
            case 'a':
            case 's':
            case 'd':
                handle_move(maze, input);
                break;
                
            case 'm':
                display_map(maze);
                break;
                
            case 'q':
                game_running = 0;
                printf("Game exited\n");
                break;
                
            default:
                printf("Invalid command! Valid commands: W/A/S/D/M/Q\n");
                break;
        }

        // Check win condition
        if (maze->playerX == maze->exitX && maze->playerY == maze->exitY)
        {
            printf("\n=== Congratulations! You escaped the maze! ===\n");
            game_running = 0;
        }
    }
}