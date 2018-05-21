#include <xseed-common/config.h>
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <xseed-common/vcs_getopt.h>
#else
#include <unistd.h>
#include <getopt.h>
#endif

void display_help (char *program_name, char *message, struct option long_opts, char **opt_descriptions)
{
    printf("Usage: %s\n", program_name);
    printf("\n%s\n", message);
}
