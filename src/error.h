#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    success,
    out_of_memory
} error_t;

static void *
malloc_or_error(const size_t size)
{
    void *ptr = malloc(size);
    if(!ptr) {
        fprintf(stderr, "Error: could not allocate %lu bytes.\n", size);
        exit(out_of_memory);
    }
    return ptr;
}

#endif // ERROR_H
