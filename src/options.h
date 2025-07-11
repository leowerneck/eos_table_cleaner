#ifndef OPTIONS_H
#define OPTIONS_H

typedef enum
{
    SMOOTH_INVALID = -1,
    SMOOTH_NONE,
    SMOOTH_DERIVS_ONLY,
    SMOOTH_HYDRO_ONLY,
    SMOOTH_ALL,
} smoother_t;

typedef enum
{
    DERIVS_INVALID = -1,
    DERIVS_SMOOTH,
    DERIVS_RECOMPUTE,
    DERIVS_DO_NOTHING,
} derivs_t;

typedef struct
{
    char       input_table_path[1024];
    char       output_table_path[1034];
    smoother_t smoother;
    derivs_t   derivs;
} options_t;

options_t parse_cmd_args(int argc, char **argv);

#endif // OPTIONS_H
