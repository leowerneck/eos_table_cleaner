#include "utils.h"

static void
generic_message(FILE *fp, const error_t key, const char *prefix, const char *format, va_list args)
{
    fputs(prefix, fp);

    vfprintf(fp, format, args);
    fflush(fp);
    va_end(args);

    if(key != SUCCESS) {
        exit(key);
    }
}

void
info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    generic_message(stdout, SUCCESS, "(info) ", format, args);
}

void
debug(const char *format, ...)
{
#ifndef NDEBUG
    va_list args;
    va_start(args, format);
    generic_message(stderr, SUCCESS, "(debug) ", format, args);
#else
    (void)format; // Flag format as intentionally unused in release builds
#endif
}

void
warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    generic_message(stderr, SUCCESS, "(warning) ", format, args);
}

void
error(const error_t key, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    generic_message(stderr, key, "(error) ", format, args);
}

void *
malloc_or_error(const size_t size)
{
    void *ptr = malloc(size);
    if(!ptr) {
        error(OUT_OF_MEMORY, "Could not allocate %lu bytes.\n", size);
    }
    return ptr;
}
