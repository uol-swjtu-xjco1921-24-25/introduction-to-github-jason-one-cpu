#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdbool.h>

// Utility functions
bool safe_malloc(void **ptr, size_t size);
void handle_error(const char *message, int code);

#endif