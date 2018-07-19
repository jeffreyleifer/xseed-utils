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
bool check_file (struct warn_options_s *options, FILE *input, char *schema_file_name, char *file_name, uint8_t verbose)
{

    bool valid_file = false;
    bool valid_header = false;
    bool valid_ident = false;
    bool valid_extra_header = false;
    bool valid_payload = false;


    int file_len = xseed_file_length(input);
    ms_log(0,"\n\nReading file %s\n",file_name);
    ms_log(0,"Record length of %d found, starting verification...\n",file_len);


    for (long file_pos = ftell(input);file_len > file_pos; file_pos = ftell(input))
    {
        uint8_t identifier_len = 0;
        uint16_t extra_header_len = 0;
        uint32_t payload_len = 0;
        uint8_t payload_fmt = 0;

        ms_log (0, "---------------------------------------------\n");
        ms_log (0,"---Starting Fixed Header verification---\n");
        valid_header = check_header(options, input, file_len, &file_pos, &identifier_len, &extra_header_len, &payload_len, &payload_fmt,verbose);

        if(valid_header)
        {
            printf("Header is valid!\n");
        }
        else
        {
            printf("Error! Fixed Header is not valid!\n");
        }

        //TODO Do I need?
        valid_ident = check_identifier(options, input, identifier_len);
        if(!valid_ident)
        {
            printf("Error parsing identifier\n");
        }
        ms_log(0,"---Completed Header verification---\n");


        ms_log(0,"---Starting Extra Header verification---\n");
        valid_extra_header = check_extra_headers(options, schema_file_name, input, extra_header_len, verbose);
        if(valid_extra_header && schema_file_name != NULL && extra_header_len > 0)
        {
            printf("Extra Header is valid!\n");

        }
        if(!valid_extra_header)
        {
            printf("Error! Extra Header not valid under provided schema!\n");
        }
        ms_log(0,"---Completed Extra Header verification---\n");


        ms_log (0, "---Started Data Payload Verification\n");
        valid_payload = check_payloads(options, input, payload_len, payload_fmt, file_name, verbose);
        if(valid_payload)
        {
            printf("Payload is valid!\n");
        }
        else
        {
            printf("Error! Payload not valid!\n");
        }
        ms_log (0, "---Completed Data Payload Verification---\n");
    }

    //TODO needs to be conditional (valid_file)
    return (valid_header && valid_ident && valid_extra_header && valid_payload);
}
