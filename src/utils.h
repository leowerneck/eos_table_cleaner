/**
 * @file utils.h
 * @author Leo Werneck
 *
 * @brief Utility functions and definitions for error handling and logging.
 *
 * This file provides common utility functions for logging messages at different
 * levels (info, warning, debug, error), a custom error type enum, and a macro
 * for simplified error reporting with file, line, and function context. It also
 * includes a safe memory allocation function that handles potential errors.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Defines standard error codes used throughout the application.
 */
typedef enum
{
    SUCCESS,                      ///< Operation completed successfully.
    OUT_OF_MEMORY,                ///< Failed to allocate memory.
    FILE_OPEN_FAILED,             ///< Failed to open a file.
    HDF5_DATASET_INVALID_NDIMS,   ///< HDF5 dataset has an invalid number of dimensions.
    HDF5_DATASET_NOT_FOUND,       ///< HDF5 dataset could not be found.
    HDF5_DATASET_READ_FAILED,     ///< Failed to read from an HDF5 dataset.
    HDF5_DATASET_WRITE_FAILED,    ///< Failed to write to an HDF5 dataset.
    HDF5_DATASET_CREATE_FAILED,   ///< Failed to create an HDF5 dataset.
    HDF5_DATASPACE_CREATE_FAILED, ///< Failed to create an HDF5 dataspace.
    UNKNOWN_OPTION,
    INVALID_SMOOTHER,
    INVALID_DERIVS,
} error_t;

/**
 * @brief Logs an informational message to standard output.
 *
 * @param format The format string (printf-style).
 * @param ... Optional arguments for the format string.
 */
void info(const char *format, ...);

/**
 * @brief Logs a warning message to standard error.
 *
 * @param format The format string (printf-style).
 * @param ... Optional arguments for the format string.
 */
void warn(const char *format, ...);

/**
 * @brief Logs a debug message to standard error (conditionally compiled only when NDEBUG is *not* defined).
 *
 * @param format The format string (printf-style).
 * @param ... Optional arguments for the format string.
 *
 * @note Debug messages are typically only enabled in debug builds.
 */
void debug(const char *format, ...);

/**
 * @brief Logs an error message to standard error and exits the program.
 *
 * @param key The error code from the error_t enum.
 * @param format The format string (printf-style).
 * @param ... Optional arguments for the format string.
 *
 * @note This function terminates the program execution with the provided error code.
 */
void error(const error_t key, const char *format, ...);

/**
 * @brief Allocates memory using malloc and handles potential allocation failures.
 *
 * @param size The number of bytes to allocate.
 *
 * @return A pointer to the allocated memory block.
 *
 * @note If malloc fails, this function logs an OUT_OF_MEMORY error and terminates the program.
 */
void *malloc_or_error(const size_t size);

/**
 * @brief Macro for simplified error reporting.
 *
 * This macro automatically includes the file name, line number, and function name
 * in the error output before calling the error() function.
 *
 * @param key The error_t code.
 * @param format The format string for the error message.
 * @param ... Optional arguments for the format string.
 */
#define ERROR(key, format, ...)                                                                                        \
    fprintf(stderr, "(error) File: %s\n(error) Line: %d\n(error) Func: %s\n", __FILE__, __LINE__, __func__);           \
    error(key, format, ##__VA_ARGS__)

#define CHECK_FINITE(new, old, func)                                                                                   \
    if(is##func(new)) {                                                                                                \
        if(is##func(old)) {                                                                                            \
            warn("%llu: BAD - both are %s\n", index, #func);                                                           \
        }                                                                                                              \
        else {                                                                                                         \
            warn("%llu: WORSE - new is %s\n", index, #func);                                                           \
        }                                                                                                              \
    }

#define CHECK_BOUNDS(new, old, op, bound)                                                                              \
    if((new)op(bound)) {                                                                                               \
        if((old)op(bound)) {                                                                                           \
            warn("%llu: BAD - both are %s %g\n", index, #op, (bound));                                                 \
        }                                                                                                              \
        else {                                                                                                         \
            warn("%llu: WORSE - new is %s %g\n", index, #op, (bound));                                                 \
        }                                                                                                              \
    }

#endif // UTILS_H
