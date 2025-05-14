#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Safely allocate memory with error checking
 * @param ptr Double pointer to receive allocated memory
 * @param size Allocation size in bytes
 * @return true if successful, false otherwise
 */
bool safe_malloc(void **ptr, size_t size)
{
    *ptr = malloc(size);
    return *ptr != NULL;
}

/**
 * @brief Unified error handler with exit codes
 * @param message Error description
 * @param code Exit code from specification
 */
void handle_error(const char *message, int code)
{
    fprintf(stderr, "Error: %s\n", message);
    exit(code);
}