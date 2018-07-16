#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "validator.h"
#include "warnings.h"


/*! @brief Read identifier length into buffer
 *
 *  @param[in] options -W cmd line warn options (currently not implemented)
 *  @param[in] input file pointer to miniSEED file
 *  @param[in] identifier_len
 *
 */
bool check_identifier(struct warn_options_s *options, FILE *input, uint8_t identifier_len)
{
    char *buffer = (char *) calloc(identifier_len + 1, sizeof(char));
    bool output = true;

    if ( identifier_len > fread(buffer, sizeof(char), identifier_len, input))
    {
        printf("Error: EOF reached reading identifier_len into buffer, please double check input record\n");
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
