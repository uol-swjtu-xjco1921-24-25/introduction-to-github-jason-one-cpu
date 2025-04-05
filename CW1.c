#include <stdio.h>
#include <stdlib.h>

// Define the maze structure
typedef struct
{
    char grid[100][100]; 
    int width;          
    int height;        
    int playerX;        
    int playerY;         
} Maze;

// function declaration
Maze load_maze(const char *filename);    // Load maze file
void display_map(const Maze *maze);      // Display map (including player position X)
int handle_move(Maze *maze, char input); // Process move instruction
void run_game_loop(Maze *maze);          // Main loop

int main(int argc, char *argv[])
{
    // Check command line parameters
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <maze_file>\n", argv[0]);
        return 1;
    }

    // Loading maze
    Maze maze = load_maze(argv[1]);

    // Start game loop
    run_game_loop(&maze);

    return 0;
}

// The following is the function framework
Maze load_maze(const char *filename)
{
    // TODO: Read files, parse maze dimensions and contents, and initialize structures
    Maze maze;
    return maze;
}

void display_map(const Maze *maze)
{
    // TODO: Print the map and the player position is shown as X
}

int handle_move(Maze *maze, char input)
{
    // TODO: Update player position based on input, return 0 (failure) or 1 (success)
    return 0;
}

void run_game_loop(Maze *maze)
{
    // TODO: Loop reads user input and handles logic such as move, display, exit, etc
}