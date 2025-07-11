#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "options.h"
#include "utils.h"

static bool
streq(const char *str1, const char *str2)
{
    return !strcmp(str1, str2);
}

static void
strlower(char *str)
{
    while(*str != '\0') {
        *str = tolower(*str);
        str++;
    }
}

static smoother_t
get_smoother_from_str(char *str)
{
    if(streq(str, "all")) {
        return SMOOTH_ALL;
    }
    else if(streq(str, "derivs")) {
        return SMOOTH_DERIVS_ONLY;
    }
    else if(streq(str, "hydro")) {
        return SMOOTH_HYDRO_ONLY;
    }
    else if(streq(str, "none")) {
        return SMOOTH_NONE;
    }
    else {
        return SMOOTH_INVALID;
    }
}

static char *
smoother_to_str(const smoother_t smoother)
{
    switch(smoother) {
        case SMOOTH_ALL:
            return "all";
        case SMOOTH_DERIVS_ONLY:
            return "derivs only";
        case SMOOTH_HYDRO_ONLY:
            return "hydro only";
        case SMOOTH_NONE:
            return "none";
        default:
            return "invalid smoothing option";
    }
}

static derivs_t
get_derivs_from_str(char *str)
{
    if(streq(str, "smooth")) {
        return DERIVS_SMOOTH;
    }
    else if(streq(str, "recompute")) {
        return DERIVS_RECOMPUTE;
    }
    else if(streq(str, "none")) {
        return DERIVS_DO_NOTHING;
    }
    else {
        return DERIVS_INVALID;
    }
}

static char *
derivs_to_str(const derivs_t derivs)
{
    switch(derivs) {
        case DERIVS_SMOOTH:
            return "smooth";
        case DERIVS_RECOMPUTE:
            return "recompute";
        case DERIVS_DO_NOTHING:
            return "do nothing";
        default:
            return "invalid derivative option";
    }
}

options_t
parse_cmd_args(int argc, char **argv)
{
    options_t options = {0};
    options.smoother  = SMOOTH_DERIVS_ONLY;
    options.derivs    = DERIVS_SMOOTH;

    for(int n = 1; n < argc; n++) {
        char *opt = argv[n];

        if(streq(opt, "--output") || streq(opt, "-o")) {
            snprintf(options.output_table_path, 1024, "%s", argv[++n]);
        }
        else if(streq(opt, "--smoothing") || streq(opt, "-s")) {
            opt = argv[++n];
            strlower(opt);

            options.smoother = get_smoother_from_str(opt);
            if(options.smoother == SMOOTH_INVALID) {
                error(INVALID_SMOOTHER, "Unknown smoothing option '%s'\n", opt);
            }
        }
        else if(streq(opt, "--derivs") || streq(opt, "d")) {
            opt = argv[++n];
            strlower(opt);

            options.derivs = get_derivs_from_str(opt);
            if(options.derivs == DERIVS_INVALID) {
                error(INVALID_DERIVS, "Unknown derivative option '%s'\n", opt);
            }
        }
        else {
            debug("opt = %s", opt);
            if(opt[0] == '-') {
                error(UNKNOWN_OPTION, "Unknown option '%s'\n", opt);
            }
            if(options.input_table_path[0] != '\0') {
                error(UNKNOWN_OPTION, "Unexpected positional argument '%s'\n", opt);
            }
            snprintf(options.input_table_path, 1024, "%s", opt);
        }
    }

    if(options.output_table_path[0] == '\0') {
        // User didn't provide an output table path. Set it to default.
        // Remove .h5 from input file
        char *dot = strrchr(options.input_table_path, '.');
        if(dot) {
            *dot = '\0';
        }
        snprintf(options.output_table_path, 1034, "%s_clean.h5", options.input_table_path);
        *dot = '.';
    }

    info("Input table path  : %s\n", options.input_table_path);
    info("Output table path : %s\n", options.output_table_path);
    info("Smoothing option  : %s\n", smoother_to_str(options.smoother));
    info("Derivative option : %s\n", derivs_to_str(options.derivs));

    return options;
}
