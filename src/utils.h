#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    SUCCESS,
    OUT_OF_MEMORY,
    FILE_OPEN_FAILED,
    HDF5_DATASET_INVALID_NDIMS,
    HDF5_DATASET_NOT_FOUND,
    HDF5_DATASET_READ_FAILED,
    HDF5_DATASET_WRITE_FAILED,
    HDF5_DATASET_CREATE_FAILED,
    HDF5_DATASPACE_CREATE_FAILED,
} error_t;

void  info(const char *format, ...);
void  warn(const char *format, ...);
void  debug(const char *format, ...);
void  error(const error_t key, const char *format, ...);
void *malloc_or_error(const size_t size);

#define ERROR(key, format, ...)                                                                                        \
    fprintf(stderr, "(error) File: %s\n(error) Line: %d\n(error) Func: %s\n", __FILE__, __LINE__, __func__);           \
    error(key, format, ##__VA_ARGS__)

#endif // UTILS_H
