#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libmseed.h>
#include "validator.h"
#include "warnings.h"

bool check_payloads(struct warn_options_s *options, FILE *input, uint32_t payload_len, uint8_t payload_fmt,char* file_name)
{
    bool answer = true;
    char *buffer = (char *) calloc(payload_len +1, sizeof(char));

    if (payload_len > fread(buffer, sizeof(char), payload_len, input))
    {
        answer = false;
    }

    switch (payload_fmt)
    {
        case 0: /* text */
            /* if this was succesfull but already made false we need to keep */
            answer = check_payload_text(options, payload_len, buffer)? answer : false;
            break;
        case 1: /* 16-bit, integer, little-endian*/
        case 3: /* 32-bit, integer, little-endian*/
        case 4: /* IEEE 32-bit floats, little-endian */
        case 5: /* IEEE 64-bit floats (double), little-endian */
        case 10: /* Steim-1 integer compressin, big-endian */
        case 11: /* Steim-2 integer compressin, big-endian */
        case 19: /* Steim-3 integer compressin, big-endian */
        case 53: /* 32-bit integer, little-endian, general compressor */
        case 54: /* 32-bit IEEE floats, little-endian, general compressor */
        case 55: /* 64-bit IEEE floats, little-endian, general compressor */
            break;
        default:
            /* invalid payload type */
            answer = false;
            break;
    }
    if (buffer)
    {
        free(buffer);
    }
    return answer;
}
