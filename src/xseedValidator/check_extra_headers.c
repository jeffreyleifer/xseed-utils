#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "validator.h"
#include "warnings.h"

bool check_extra_headers(FILE *input, uint16_t extra_header_len)
{
    char *buffer = (char *) calloc(extra_header_len + 1, sizeof(char));

    if ( extra_header_len > fread(buffer, sizeof(char), extra_header_len, input))
    {
        /* TODO warn reached end of file */
        free(buffer);
        return false;
    }

    /*TODO check is valid json */
    /*TODO other checks */

    free (buffer);
    return true;
}
