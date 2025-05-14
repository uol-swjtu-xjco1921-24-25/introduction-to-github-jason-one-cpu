#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Maze cell states
#define WALL 0  // Wall block
#define PATH 1  // Walkable path
#define START 2 // Starting point
#define END 3   // Exit point

// Direction enumeration
typedef enum
{
    UP = 0,    // Move up
    RIGHT = 1, // Move right
    DOWN = 2,  // Move down
    LEFT = 3   // Move left
} Direction;

// Coordinate structure
typedef struct
{
    int x, y; // Horizontal and vertical positions
} Point;

// Stack node for DFS
typedef struct StackNode
{
    Point pos;              // Current position
    struct StackNode *next; // Next node pointer
} StackNode;

/* Stack operations */

// Push position to stack
void pushStack(StackNode **top, Point pos)
{
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    node->pos = pos;
    node->next = *top;
    *top = node;
}

// Pop position from stack
Point popStack(StackNode **top)
{
    if (*top == NULL)
    {
        Point invalid = {-1, -1};
        return invalid;
    }

    StackNode *temp = *top;
    Point pos = temp->pos;
    *top = temp->next;
    free(temp);
    return pos;
}

// Check empty stack
int isStackEmpty(StackNode *top)
{
    return top == NULL;
}

/* Maze generation */

// Generate maze using DFS algorithm
void generateMaze(int **maze, int height, int width)
{
    // Initialize all cells as walls
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            maze[i][j] = WALL;
        }
    }

    StackNode *stack = NULL;
    // Random start position with odd coordinates
    int startX = 1 + 2 * (rand() % ((width - 2) / 2));
    int startY = 1 + 2 * (rand() % ((height - 2) / 2));
    maze[startY][startX] = PATH;

    Point start = {startX, startY};
    pushStack(&stack, start);

    // Direction offsets (up, right, down, left)
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};

    while (!isStackEmpty(stack))
    {
        Point current = popStack(&stack);
        // Shuffle directions randomly
        int directions[4] = {0, 1, 2, 3};
        for (int i = 3; i > 0; i--)
        {
            int r = rand() % (i + 1);
            int temp = directions[i];
            directions[i] = directions[r];
            directions[r] = temp;
        }

        // Explore neighboring cells
        for (int i = 0; i < 4; i++)
        {
            int dir = directions[i];
            int nx = current.x + dx[dir] * 2;
            int ny = current.y + dy[dir] * 2;

            // Check valid unvisited cell
            if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && maze[ny][nx] == WALL)
            {
                // Carve path between cells
                maze[current.y + dy[dir]][current.x + dx[dir]] = PATH;
                maze[ny][nx] = PATH;

                Point newPos = {nx, ny};
                pushStack(&stack, newPos);
                pushStack(&stack, current);
                break;
            }
        }
    }

    // Set fixed start/end positions
    maze[1][1] = START;
    maze[height - 2][width - 2] = END;
}

/* Maze solving */

// Solve maze using BFS approach
int solveMaze(int **maze, int height, int width)
{
    StackNode *queue = NULL;
    // Create visited tracking array
    int **visited = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++)
    {
        visited[i] = (int *)calloc(width, sizeof(int));
    }

    Point start = {1, 1};
    pushStack(&queue, start);
    visited[1][1] = 1;

    // Movement direction offsets
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};

    while (!isStackEmpty(queue))
    {
        Point current = popStack(&queue);
        // Check exit found
        if (maze[current.y][current.x] == END)
        {
            for (int i = 0; i < height; i++)
                free(visited[i]);
            free(visited);
            return 1;
        }

        // Explore adjacent cells
        for (int i = 0; i < 4; i++)
        {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                (maze[ny][nx] == PATH || maze[ny][nx] == END) && !visited[ny][nx])
            {
                Point newPos = {nx, ny};
                pushStack(&queue, newPos);
                visited[ny][nx] = 1;
            }
        }
    }

    // Cleanup and return unsolvable
    for (int i = 0; i < height; i++)
        free(visited[i]);
    free(visited);
    return 0;
}

/* File handling */

// Save maze to text file
void saveMazeToFile(int **maze, int height, int width, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("File error: %s\n", filename);
        return;
    }

    // Convert maze codes to characters
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            switch (maze[i][j])
            {
            case WALL:
                fputc('#', file);
                break;
            case PATH:
                fputc(' ', file);
                break;
            case START:
                fputc('S', file);
                break;
            case END:
                fputc('E', file);
                break;
            }
        }
        fputc('\n', file);
    }
    fclose(file);
}

/* Main function */

int main()
{
    char filename[] = "maze.txt";
    int height = 20;
    int width = 20;

    // Validate dimension limits
    if (height < 5 || height > 100 || width < 5 || width > 100)
    {
        printf("Invalid dimensions (5-100)\n");
        return 1;
    }

    // Ensure odd dimensions
    if (height % 2 == 0)
        height++;
    if (width % 2 == 0)
        width++;

    // Allocate 2D maze array
    int **maze = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++)
    {
        maze[i] = (int *)malloc(width * sizeof(int));
    }

    // Initialize random seed
    srand(time(NULL) ^ (unsigned int)&height ^ (unsigned int)clock());

    // Generate until solvable maze
    do
    {
        generateMaze(maze, height, width);
    } while (!solveMaze(maze, height, width));

    // Save and cleanup
    saveMazeToFile(maze, height, width, filename);
    printf("Maze saved to %s\n", filename);

    for (int i = 0; i < height; i++)
        free(maze[i]);
    free(maze);
    return 0;
}