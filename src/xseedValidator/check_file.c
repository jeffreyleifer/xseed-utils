#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <xseed-common/files.h>
#include <libmseed.h>
#include "validator.h"
#include "warnings.h"



/*! @brief Perform all verification test
 *
 *  @param[in] options -W cmd line warn options (currently not implemented)
 *  @param[in] input file pointer to miniSEED file
 *  @param[in] schema_file_name json file path parsed from cmd line
 *  @param[in] file_name miniSEED file path parsed from cmd line
 *
 */
bool check_file (struct warn_options_s *options, FILE *input, char *schema_file_name, char *file_name)
{

    bool valid_file = false;

    int file_len = xseed_file_length(input);
    ms_log(0,"\n\nReading file %s\n",file_name);
    ms_log(0,"Record length of %d found, starting verification...\n",file_len);


    for (long file_pos = ftell(input);file_len > file_pos; file_pos = ftell(input))
    {
        uint8_t identifier_len = 0;
        uint16_t extra_header_len = 0;
        uint32_t payload_len = 0;
        uint8_t payload_fmt = 0;


        valid_file = check_header(options, input, file_len, &file_pos, &identifier_len, &extra_header_len, &payload_len, &payload_fmt);

        if(valid_file)
        {
            printf("Header is valid!\n");
        }
        else
        {
            printf("Error! Header is not valid!\n");
        }

        valid_file = check_identifier(options, input, identifier_len);
        valid_file = check_extra_headers(options, schema_file_name, input, extra_header_len);
        valid_file = check_payloads(options, input, payload_len, payload_fmt,file_name);
    }

    //TODO needs to be conditional (valid_file)
    return true;
}
