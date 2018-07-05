#define XSEED_STATIC_HEADER_LEN 40
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <libmseed.h>
#include "validator.h"
#include "warnings.h"
#include <xseed-common/constants.h>
#include <xseed-common/array.h>



/*
 *

  * Three levels are recognized:
 * 0  : Normal log messages, printed using log_print with logprefix
 * 1  : Diagnostic messages, printed using diag_print with logprefix
 * 2+ : Error messagess, printed using diag_print with errprefix



A record is composed of a header followed by a time series data payload. The byte order of binary
fields in the header must be least significant byte first (little endian).
The total length of a record is variable and is the sum of 40 (length of fixed section of header),
field 10 (length of identifier), field 11 (length of extra headers), field 12 (length of payload).
Field  Field name                            Type       Length    Offset  Content
1      Record header indicator (‘MS’)        CHAR       2         0       ASCII ‘MS’
2      Format version (3)                    UINT8      1         2
3      Flags                                 UINT8      1         3
       Record start time
4a       Nanosecond (0 - 999999999)          UINT32     4         4
4b       Year (0 - 65535)                    UINT16     2         8
4c       Day-of-year  (1 - 366)              UINT16     2         10
4d       Hour (0 - 23)                       UINT8      1         12
4e       Minute (0 - 59)                     UINT8      1         13
4f       Second (0 - 60)                     UINT8      1         14
5      Data encoding format                  UINT8      1         15
6      Sample rate/period                    FLOAT64    8         16
7      Number of samples                     UINT32     4         24
8      CRC of the record                     UINT32     4         28
9      Data publication version              UINT8      1         32
10     Length of identifier in bytes         UINT8      1         33
11     Length of extra headers in bytes      UINT16     2         34
12     Length of data payload in bytes       UINT32     2         36
13     Time series identifier                CHAR       V         40      URN identifier
14     Extra header fields                   JSON       V         40 + field 10
15     Data payload                          encoded    V         40 + field 10 + field 11
All length values are specified in bytes, which are assumed to be 8-bits in length
​*/



bool check_header(struct warn_options_s *options, FILE *input_file, long file_len, long *file_pos, uint8_t *identifier_len, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt)
{

    //prime buffer
    char buffer[XSEED_STATIC_HEADER_LEN];

    //read values into buffer, dummy EOF check
    ms_log (0, "---------------------------------------------\n");
    ms_log (0, "Reading header for verification in little endian format...\n");
    if ( XSEED_STATIC_HEADER_LEN != fread((void *)buffer, sizeof(char), XSEED_STATIC_HEADER_LEN, input_file))
    {
        ms_log (2, "Error: File size mismatch, please double check input record\n");
    }



    //Check Header flags
    char flag[3];
    flag[0] = (char)buffer[0];
    flag[1] = (char)buffer[1];


    ms_log (0, "Checking Header Flag value: %c%c\n", flag[0],flag[1]);
    //if ((char)buffer[0] != 'M' && (char)buffer[1] != 'S')
    if(!(flag[0] == 'M' && flag[1] == 'S'))
    {
        ms_log(2,"Error: Header Flag Value Incorrect ('MS' currently is the only valid flag)\n");
    }

    //Check MS version
    uint8_t msVersion = (uint8_t)buffer[2];
    ms_log (0, "Checking File Version value: %d\n", msVersion);
    if (3 != msVersion)
    {
        ms_log(2,"Error: Header Version Value Incorrect ('3' currently is the only supported MS version)\n");
    }


    //Check for valid year range
    // assuming you have read your bytes little-endian
    //TODO try casting with both uint16_t or uint8_t
    uint16_t year = (uint8_t )buffer[8] | (uint8_t)buffer[9] << 8;
    ms_log (0, "Checking Year value: %d\n", year);
    if(year < 0 || year > 65535)
    {
       ms_log(2,"Error : Year value out of range (0-65535)\n");
    }


    //Check for Valid Day-of-Year
    //assuming you have read your bytes little-endian
    uint16_t doy = (uint8_t )buffer[10] | (uint8_t)buffer[11] << 8;
    ms_log (0, "Checking Day of Year value: %d\n", doy);
    if (366 < doy || 1 > doy)
    {
        printf("");
        ms_log (2, "Error, Day Of Year value out of range (1-366)\n");
    }
    /*  Dylans way
     * //uint16_t doy = buffer[10] + (buffer[11]*(256)
    uint16_t days;
     My way
    days = (uint8_t )buffer[10] | (uint16_t)buffer[11] << 8;
    */


    //Check for valid hour range
    uint8_t hours = (uint8_t)buffer[12];
    ms_log (0, "Checking Hours value: %d\n", hours);
    if ( hours < 0 || hours > 23)
    {
        ms_log (2, "Error, Hours value out of range (0-23)\n");
    }

    //Check for Valid min rage
    uint8_t mins = (uint8_t)buffer[13];
    ms_log (0, "Checking Mins value: %d\n", mins);
    if ( mins < 0 || mins > 59)
    {
        ms_log (2, "Error, Mins value out of range (0-59)\n");
    }

    //Check for seconds range
    uint8_t secs = (uint8_t)buffer[14];
    ms_log (0, "Checking Secs value: %d\n", secs);
    if ( secs < 0 || secs > 60 )
    {
        ms_log (2, "Error, Secs value out of range (1-366)\n");
    }


    //Dylans way
    //  uint32_t nanoseconds = buffer[12] + buffer[13]*(0xFF+1) +buffer[14]*(0xFFFF+1) + buffer[15]*(0xFFFFFF+1);

    //My way
    // assuming you have read your bytes little-endian
    uint32_t nanoseconds = ((uint8_t)buffer[4] | ((uint8_t)buffer[5] << 8) | ((uint8_t)buffer[6] << 16) | ((uint8_t)buffer[7] << 24));
    ms_log (0, "Checking Nanoseconds value: %d\n", nanoseconds);
    if (999999999 < nanoseconds)
    {
        /*TODO to many nanoseconds */
    }


    //Check for Payload type
    uint8_t payload = (uint8_t)buffer[15];

    *payload_fmt = payload;

    ms_log(0, "Checking Payload Flag: %d\n", payload);
    switch (payload)
    {
        case 0:
             ms_log(0, "Payload flag indicates TEXT payload\n");
             break;
        case 1: /* 16-bit, integer, little-endian*/
            ms_log(0, "Payload flag indicates 16-bit, integer, little-endian payload\n");
            break;
        case 3: /* 32-bit, integer, little-endian*/
            ms_log(0, "Payload flag indicates 32-bit, integer, little-endian payload\n");
            break;
        case 4: /* IEEE 32-bit floats, little-endian */
            ms_log(0, "Payload flag indicates IEEE 32-bit floats, little-endian payload\n");
            break;
        case 5: /* IEEE 64-bit floats (double), little-endian */
            ms_log(0, "Payload flag indicates IEEE 64-bit floats (double), little-endian payload\n");
            break;
        case 10: /* Steim-1 integer compressin, big-endian */
            ms_log(0, "Steim-1 integer compression, big-endian\n");
            break;
        case 11: /* Steim-2 integer compressin, big-endian */
            ms_log(0, "Steim-2 integer compression, big-endian\n");
            break;
        case 16: /* Steim-2 integer compressin, big-endian */
            ms_log(0, "CDSN 16 bit gain ranged\n");
            break;
        case 19: /* Steim-3 integer compressin, big-endian */
            ms_log(0, "Steim-3 integer compression, big-endian\n");
            break;
        case 53: /* 32-bit integer, little-endian, general compressor */
            ms_log(0, "32-bit integer, little-endian, general compressor\n");
            break;
        case 54: /* 32-bit IEEE floats, little-endian, general compressor */
            ms_log(0, "32-bit IEEE floats, little-endian, general compressor\n");
            break;
        case 55: /* 64-bit IEEE floats, little-endian, general compressor */
            ms_log(0, "64-bit IEEE floats, little-endian, general compressor\n");
            break;
        default:/* invalid payload type */
            ms_log(0, "Error: Payload Type Flag is Invalid!\n");
            break;
    };


    //TODO fix this, it doesn't convert to decimal numbers correctly
    /*convert to float64 */
    double sample_rate;
    buffer_to_number(buffer+16, sizeof(double), XSEED_DOUBLE, &sample_rate);
    ms_log(0, "Checking sample rate value: %f\n", sample_rate);


    //TODO check below values for (basic) validity
    //uint32_t number_samples_old = buffer[24] + buffer[25]*(0xFF+1) +buffer[26]*(0xFFFF+1) + buffer[27]*(0xFFFFFF+1);
    uint32_t number_samples = ((uint8_t)buffer[24] | ((uint8_t)buffer[25] << 8) | ((uint8_t)buffer[26] << 16) | ((uint8_t)buffer[27] << 24));
    ms_log(0, "Checking number of samples value: %d\n", number_samples);

    //uint32_t CRC_old = buffer[28] + buffer[29]*(0xFF+1) +buffer[30]*(0xFFFF+1) + buffer[31]*(0xFFFFFF+1);
    uint32_t CRC = ((uint8_t)buffer[28]) | ((uint8_t)buffer[29] << 8) | ((uint8_t)buffer[30] << 16) | ((uint8_t)buffer[31]) << 24 ;
    //TODO Use libmseed to verify the CRC value
    ms_log(0, "*TODO* Checking CRC value: %d\n",CRC);

    uint8_t dataPubVersion = (uint8_t)buffer[32];
    *identifier_len = (uint8_t)buffer[33];
    ms_log(0, "Checking Data Publication Version value: %d\n",dataPubVersion);


    uint16_t extra_header_l = (uint8_t )buffer[34] | (uint8_t)buffer[35] << 8;
    ms_log(0, "Checking Extra Header Length value: %d\n",extra_header_l);
    uint32_t payload_l = ((uint8_t)buffer[36] | ((uint8_t)buffer[37] << 8) | ((uint8_t)buffer[38] << 16) | ((uint8_t)buffer[39] << 24));
    ms_log(0, "Checking Payload Length value: %d\n",payload_l);

    //assign to output values
    *payload_fmt = payload;
    *extra_header_len =  extra_header_l;
    *payload_len = payload_l;

    return true;
}
