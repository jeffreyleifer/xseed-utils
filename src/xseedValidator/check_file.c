#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <xseed-common/files.h>
#include "validator.h"

bool check_file (FILE *input)
{

    int file_len = xseed_file_length(input);
    for (long file_pos = ftell(input);file_len > file_pos; file_pos = ftell(input))
    {
        uint16_t extra_header_len = 0;
        uint32_t payload_len = 0;
        uint8_t payload_fmt = 0;
        check_header(input, file_len, &file_pos, &extra_header_len, &payload_len, &payload_fmt);
        check_extra_headers(input, extra_header_len);
        check_payloads(input, payload_len, payload_fmt);
    }
    return true;
}
