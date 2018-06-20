#include <xseed-common/config.h>
#include "cmd_opt.h"
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <xseed-common/vcs_getopt.h>
#else
#include <unistd.h>
#include <getopt.h>
#endif

void display_help (char *program_name, char *message, const struct xseed_option_s *opts)
{
    printf("Usage: %s\n", program_name);
    printf("\n%s\n", message);

    const struct xseed_option_s *opt_ptr = opts;
    int max_option_len = 10;
    for (; opt_ptr != NULL && (0 != opt_ptr->short_option || 0 != opt_ptr->long_option || 0 != opt_ptr->description || 0 != opt_ptr->variable_to_fill || 0 != opt_ptr->argument_type);opt_ptr++)
    {
        char *option_str = opt_ptr->long_option;
        printf("\t%*.*s", max_option_len, max_option_len, option_str);
        printf("\n");
    }
}


