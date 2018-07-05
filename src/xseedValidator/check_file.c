#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <xseed-common/files.h>
#include <libmseed.h>
#include "validator.h"
#include "warnings.h"

bool check_file (struct warn_options_s *options, FILE *input, char *schema_file_name, char *file_name)
{

    int file_len = xseed_file_length(input);
    ms_log(0,"\n\nReading file %s\n",file_name);
    ms_log(0,"Record length of %d found, starting verification...\n",file_len);
    for (long file_pos = ftell(input);file_len > file_pos; file_pos = ftell(input))
    {
        uint8_t identifier_len = 0;
        uint16_t extra_header_len = 0;
        uint32_t payload_len = 0;
        uint8_t payload_fmt = 0;
        check_header(options, input, file_len, &file_pos, &identifier_len, &extra_header_len, &payload_len, &payload_fmt);
        check_identifier(options, input, identifier_len);
        check_extra_headers(options, schema_file_name, input, extra_header_len);
        check_payloads(options, input, payload_len, payload_fmt,file_name);
    }
    return true;
}
