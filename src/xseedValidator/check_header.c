#define XSEED_STATIC_HEADER_LEN 40
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "validator.h"

bool check_header(FILE *input_file, long file_len, long *file_pos, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt)
{
    char buffer[XSEED_STATIC_HEADER_LEN];
    if ( XSEED_STATIC_HEADER_LEN != fread((void *)buffer, sizeof(char), XSEED_STATIC_HEADER_LEN, input_file))
    {
        /*TODO ENDOFFILE */
    }
    if (buffer[0] != 'M' && buffer[1] != 'S')
    {
        /*TODO iheader doesn't start with correct flags */
    }
    if (3 != (uint8_t)buffer[2])
    {
        /*TODO incorrect version*/
    }

    /* check flags */

    uint16_t doy = buffer[6] + (buffer[7]*(256));
    if (366 < doy || 1 > doy)
    {
        /*TODO incorrect day of year */
    }
    
    if ( 23 < (uint8_t)buffer[8])
    {
        /*TODO to many hours in a day */
    }
    if ( 59 < (uint8_t)buffer[9])
    {
        /*TODO to many minutes in an hour */
    }
    if ( 60 < (uint8_t)buffer[10])
    {
        /*TODO to many seconds in a minute */
    }
    *payload_fmt = buffer[11];
    /*TODO check payload_fmt*/

    uint32_t nanoseconds = buffer[12] + buffer[13]*(0xFF+1) +buffer[14]*(0xFFFF+1) + buffer[15]*(0xFFFFFF+1);
    if (999999999 < nanoseconds)
    {
        /*TODO to many nanoseconds */
    }

    /*TODO convert to float64 */

    uint32_t number_samples = buffer[24] + buffer[25]*(0xFF+1) +buffer[26]*(0xFFFF+1) + buffer[27]*(0xFFFFFF+1);
    uint32_t CRC = buffer[28] + buffer[29]*(0xFF+1) +buffer[30]*(0xFFFF+1) + buffer[31]*(0xFFFFFF+1);
    uint8_t version = buffer[32];
    uint8_t id_len = buffer[33];
    *extra_header_len =  buffer[34] + buffer[35]*(0xFF+1);
    *payload_len = buffer[36] + buffer[37]*(0xFF+1) +buffer[38]*(0xFFFF+1) + buffer[39]*(0xFFFFFF+1);

    return true;
}
