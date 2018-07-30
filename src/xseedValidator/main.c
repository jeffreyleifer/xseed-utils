#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <xseed-common/cmd_opt.h>
#include <xseed-common/files.h>
#include <xseed-common/xseed_string.h>
#include <libmseed.h>
#include "warnings.h"
#include "validator.h"
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <xseed-common/vcs_getopt.h>
#else
#include <unistd.h>
#include <getopt.h>
#endif




//CMD line option structure
static const struct xseed_option_s args[] = {
    {'h',    "help h", "Usage", NULL, NO_OPTARG},
    {'f',    "file f", "File to validate", NULL, MANDATORY_OPTARG},
    {'j',  "schema j", "Json Schemas", NULL, MANDATORY_OPTARG},
    {'v', "verbose v", "Verbosity level", NULL, OPTIONAL_OPTARG},
    //{'W',      NULL, "Warning flag", NULL, MANDATORY_OPTARG},
    {  0,         0, 0, 0, 0}};


//Validator Main
int main(int argc, char **argv)
{


    uint8_t verbose = 0;
    char *file_name = NULL;
    char * schema_file_name = NULL;

    //vars to store command line options/args
    char *short_opt_string = NULL;
    struct option *long_opt_array = NULL;
    int opt;
    struct warn_options_s warn_options[1];
    unsigned char display_usage = 0;

    //For warning options - not used TODO get working
    memset(warn_options, 0, sizeof(struct warn_options_s));

    //parse command line args
    xseed_get_short_getopt_string (&short_opt_string, args);
    xseed_get_long_getopt_array(&long_opt_array, args);


    //Get usage options TODO in progress
    int longindex;
    while(-1 != (opt=getopt_long(argc, argv, short_opt_string, long_opt_array, &longindex)))
    {
        switch(opt)
        {
            //int file_name_size;
            case 'f':
                file_name = strndup(optarg, 1024);
                break;
            case 'v':
                if (0 == optarg )
                {
                    verbose++;
                }
                else
                {
                    verbose = strlen(optarg) + 1;
                }
                break;
            case 'W':
                parse_warn_options(warn_options, optarg);
                break;
            case 'h':
                display_usage =1;
                break;
            case 'j':
                schema_file_name = strndup(optarg, 1024);
                break;
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
        display_help(argv[0], "Program to Validate xSEED format files", args);
        return display_usage < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
    }


    if (!xseed_file_exists(file_name))
    {
        printf("Error reading file: %s, File Not Found! \n",file_name);
        return EXIT_FAILURE;
    }


    // Open ms file as binary
    FILE *file = NULL;
    file = fopen(file_name, "r");
    // run verification tests
    bool valid = check_file(warn_options, file, schema_file_name,file_name,verbose);
    fclose(file);


    //Final program output
    //TODO if extra header schema validation fails, this still registers as a success
    if(valid)
    {
        printf("xseedValidator SUCCESS, the file %s is VALID xSEED\n\n",file_name);
    } else
    {
        printf("xseedValidator FAILED, the file %s is NOT VALID xSEED\n\n",file_name);
    }

    if (file_name)
    {
        free(file_name);
    }
    if (schema_file_name)
    {
        free(schema_file_name);
    }



    //return valid ? EXIT_SUCCESS : EXIT_FAILURE;
    return EXIT_SUCCESS;
}
