#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <xseed-common/cmd_opt.h>
#include <xseed-common/files.h>
#include "warnings.h"
#include "validator.h"

static const struct xseed_option_s args[] = {
    {'h',    "help", "Usage", NULL, NO_OPTARG},
    {'f',    "file", "File to validate", NULL, MANDATORY_OPTARG},
    {'v', "verbose", "Verbosity level", NULL, OPTIONAL_OPTARG},
    {'W',      NULL, "Warning flag", NULL, MANDATORY_OPTARG},
    {  0,         0, 0, 0, 0}};

int main(int argc, char **argv)
{
    char *short_opt_string = NULL;
    struct option *long_opt_array = NULL;
    int opt;
    int verbosity = 0;
    char *file_name = NULL;
    struct warn_options_s warn_options[1];
    unsigned char display_usage = 0;

    memset(warn_options, 0, sizeof(struct warn_options_s));

    xseed_get_short_getopt_string (&short_opt_string, args);
    xseed_get_long_getopt_array(&long_opt_array, args);
    int longindex;
    while(-1 != (opt=getopt_long(argc, argv, short_opt_string, long_opt_array, &longindex)))
    {
        switch(opt)
        {
            int file_name_size;
            case 'f':
                file_name_size = strnlen(optarg, 1024);
                file_name = (char *)calloc(file_name_size, sizeof(char));
                memcpy(file_name, optarg, file_name_size);
                break;
            case 'v':
                if (0 == optarg )
                {
                    verbosity++;
                }
                else
                {
                    verbosity = atoi(optarg);
                }
                break;
            case 'W':
                parse_warn_options(warn_options, optarg);
                break;
            case 'h':
                display_usage =1;
            default:
                display_usage++;
                break;
        }
        if (display_usage > 0)
        {
            break;
        }
    }
    if (display_usage > 0 || NULL == file_name)
    {
        display_help(argv[0], "Program to Validate mini seed format", args);
        return display_usage < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    //Does file exist
    if (!xseed_file_exists(file_name))
    {
        //exit
        return EXIT_FAILURE;
    }
    // Open file
    FILE *file = NULL;
    file = fopen(file_name, "r");
    // run tests
    bool valid = check_file(file);
    fclose(file);
    return valid ? EXIT_SUCCESS : EXIT_FAILURE;
}
