#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "validator.h"
#include "warnings.h"

bool check_identifier(struct warn_options_s *options, FILE *input, uint8_t identifier_len)
{
    char *buffer = (char *) calloc(identifier_len + 1, sizeof(char));
    bool output = true;

    if ( identifier_len > fread(buffer, sizeof(char), identifier_len, input))
    {
        /* TODO warn reached end of file */
        free(buffer);
        buffer=NULL;
        output = false;
    }

    /*TODO check the identifier */

    if(buffer)
    {
        free(buffer);
    }
    return output;
}
