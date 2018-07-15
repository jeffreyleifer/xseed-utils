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



/*!

@brief Validates a miniSEED Version 3 Header

 Specification:
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



//check_header.c functions
bool parse_header_little_endian(char *buffer, bool header_valid, uint8_t *identifier_len, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt);
bool parse_header_big_endian(char *buffer, bool header_valid, uint8_t *identifier_len, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt);

/*! @brief main validate header routine
 *
 *  @param[in] options -W cmd line warn options (currently not implemented)
 *  @param[in] input file pointer to miniSEED file
 *  @param[in] file_len overall binary length
 *  @param[in,out] file_pos pointer to current position in the file
 *  @param[out] identifier_len
 *  @param[out] extra_header_len
 *  @param[out] payload_len
 *
 *  @todo handle big endian case
 *
 */bool check_header(struct warn_options_s *options, FILE *input_file, long file_len, long *file_pos,
                      uint8_t *identifier_len, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt)
{


    bool header_valid = true;
    //prime buffer
    char buffer[XSEED_STATIC_HEADER_LEN];

    //read values into buffer, EOF check
    ms_log (0, "---------------------------------------------\n");
    ms_log (0,"---Starting fixed header verification---\n");
    if ( XSEED_STATIC_HEADER_LEN != fread((void *)buffer, sizeof(char), XSEED_STATIC_HEADER_LEN, input_file))
    {
        ms_log (2, "Error: File size mismatch, check input record\n");
        header_valid = false;
    }

    ms_log (0,"Checking host endianness...");
    bool is_big_emdian = ms_bigendianhost();
    if(is_big_emdian)
    {
        ms_log (0,"host is Big Endian\n");
        header_valid = parse_header_big_endian(buffer, header_valid, identifier_len, extra_header_len, payload_len, payload_fmt);

    } else
    {
        ms_log (0,"host is Little Endian\n");
        header_valid = parse_header_little_endian(buffer, header_valid, identifier_len, extra_header_len, payload_len, payload_fmt);
    }


    return header_valid;
}


bool parse_header_little_endian(char *buffer, bool header_valid, uint8_t *identifier_len, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt)
{

    //Check Header flags
    char flag[3];
    flag[0] = (char)buffer[0];
    flag[1] = (char)buffer[1];
    ms_log (0, "Checking Header Flag value: %c%c\n", flag[0],flag[1]);
    if(!(flag[0] == 'M' && flag[1] == 'S'))
    {
        ms_log(2,"Error: Header Flag Value Incorrect ('MS' is only valid flag)\n");
        header_valid = false;
    }

    //Check MS version
    uint8_t msVersion = (uint8_t)buffer[2];
    ms_log (0, "Checking File Version value: %d\n", msVersion);
    if (3 != msVersion)
    {
        ms_log(2,"Error: Header Version Value Incorrect ('3' currently is the only supported MS version)\n");
        header_valid = false;
    }


    //Check valid Year
    uint16_t year = (uint8_t )buffer[8] | (uint8_t)buffer[9] << 8;
    ms_log (0, "Checking Year value: %d\n", year);
    if(year < 0 || year > 65535)
    {
        ms_log(2,"Error : Year value out of range (0-65535)\n");
        header_valid = false;
    }
    //TODO Warn for data in future


    //Check valid Day-of-Year
    uint16_t doy = (uint8_t )buffer[10] | (uint8_t)buffer[11] << 8;
    ms_log (0, "Checking Day of Year value: %d\n", doy);
    if (366 < doy || 1 > doy)
    {

        ms_log (2, "Error, Day Of Year value out of range (1-366)\n");
        header_valid = false;
    }
    /*  Dylans way
     * //uint16_t doy = buffer[10] + (buffer[11]*(256)
    */


    //Check valid hour range
    uint8_t hours = (uint8_t)buffer[12];
    ms_log (0, "Checking Hours value: %d\n", hours);
    if ( hours < 0 || hours > 23)
    {
        ms_log (2, "Error, Hours value out of range (0-23)\n");
        header_valid = false;
    }

    //Check valid min rage
    uint8_t mins = (uint8_t)buffer[13];
    ms_log (0, "Checking Mins value: %d\n", mins);
    if ( mins < 0 || mins > 59)
    {
        ms_log (2, "Error, Mins value out of range (0-59)\n");
        header_valid = false;
    }

    //Check valid seconds range
    uint8_t secs = (uint8_t)buffer[14];
    ms_log (0, "Checking Secs value: %d\n", secs);
    if ( secs < 0 || secs > 60 )
    {
        ms_log (2, "Error, Secs value out of range (1-366)\n");
        header_valid = false;
    }


    //Dylans way
    //  uint32_t nanoseconds = buffer[12] + buffer[13]*(0xFF+1) +buffer[14]*(0xFFFF+1) + buffer[15]*(0xFFFFFF+1);

    //Check valid nanoseconds
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
        case 14: /* Steim-2 integer compressin, big-endian */
            ms_log(0, "Steim-2 integer compression, big-endian\n");
            break;
            //TODO verify this with chad
        case 16: /* GEOSCOPE Muxed 16/4 bit gain/exp ,little endian(?) */
            ms_log(0, "GEOSCOPE Muxed 16/4 bit gain/exp, little endian(?)\n");
            break;
        case 19: /* Steim-3 integer compressin, big-endian */
            ms_log(0, "Steim-3 integer compression, big-endian\n");
            break;
            //TODO verify this with chad
        case 30: /* SRO Gain Ranged Format, ?-endian */
            ms_log(0, "SRO Gain Ranged Format, ?-endian\n");
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
            header_valid = false;
            break;
    };



    //Get Sample Rate
    double sample_rate;
    //TODO convenience function for parsing buffer, in progress
    //buffer_to_number(buffer+16, sizeof(double), XSEED_DOUBLE, &sample_rate);
    //TODO need check for valid sample rate
    sample_rate = *((double*)((uint8_t*)buffer+16));
    if(sample_rate < 0 )
    {
        sample_rate = sample_rate * (-.01);//TODO ?????
    }
    ms_log(0, "Checking sample rate value: %f\n", sample_rate);


    //Get Number of Samples
    //TODO need check for valid number_samples
    //uint32_t number_samples = buffer[24] + buffer[25]*(0xFF+1) +buffer[26]*(0xFFFF+1) + buffer[27]*(0xFFFFFF+1);
    uint32_t number_samples = ((uint8_t)buffer[24] | ((uint8_t)buffer[25] << 8) | ((uint8_t)buffer[26] << 16) | ((uint8_t)buffer[27] << 24));
    ms_log(0, "Checking number of samples value: %d\n", number_samples);

    //Get CRC Value
    //uint32_t CRC = buffer[28] + buffer[29]*(0xFF+1) +buffer[30]*(0xFFFF+1) + buffer[31]*(0xFFFFFF+1);
    uint32_t CRC = ((uint8_t)buffer[28]) | ((uint8_t)buffer[29] << 8) | ((uint8_t)buffer[30] << 16) | ((uint8_t)buffer[31]) << 24 ;
    ms_log(0, "CRC value: 0x%0X\n",CRC);

    //Get dataPubVersion
    //TODO Check for valid dataPubVersion
    uint8_t dataPubVersion = (uint8_t)buffer[32];
    ms_log(0, "Data Publication Version value: %d\n",dataPubVersion);
    uint8_t identifier_l = (uint8_t)buffer[33];

    //Get lengths for extra header and payload
    ms_log(0, "Identifier Length value: %d\n",identifier_l);
    uint16_t extra_header_l = (uint8_t )buffer[34] | (uint8_t)buffer[35] << 8;
    ms_log(0, "Extra Header Length value: %d\n",extra_header_l);
    uint32_t payload_l = ((uint8_t)buffer[36] | ((uint8_t)buffer[37] << 8) | ((uint8_t)buffer[38] << 16) | ((uint8_t)buffer[39] << 24));
    ms_log(0, "Payload Length value: %d\n",payload_l);

    //assign to output values
    *payload_fmt = payload;
    *extra_header_len =  extra_header_l;
    *payload_len = payload_l;
    *identifier_len = identifier_l;


    return header_valid;
}

bool parse_header_big_endian(char *buffer, bool header_valid, uint8_t *identifier_len, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt)
{


   //Check Header flags
    char flag[3];
    flag[0] = (char)buffer[0];
    flag[1] = (char)buffer[1];
    ms_log (0, "Checking Header Flag value: %c%c\n", flag[0],flag[1]);
    if(!(flag[0] == 'M' && flag[1] == 'S'))
    {
        ms_log(2,"Error: Header Flag Value Incorrect ('MS' is only valid flag)\n");
        header_valid = false;
    }

    //Check MS version
    uint8_t msVersion = (uint8_t)buffer[2];
    ms_log (0, "Checking File Version value: %d\n", msVersion);
    if (3 != msVersion)
    {
        ms_log(2,"Error: Header Version Value Incorrect ('3' currently is the only supported MS version)\n");
        header_valid = false;
    }


    //Check valid Year
    uint16_t year = (uint8_t )buffer[9] | (uint8_t)buffer[8] << 8;
    ms_log (0, "Checking Year value: %d\n", year);
    if(year < 0 || year > 65535)
    {
        ms_log(2,"Error : Year value out of range (0-65535)\n");
        header_valid = false;
    }
    //TODO Warn for future data


    //Check valid Day-of-Year
    //assuming you have read your bytes big-endian
    uint16_t doy = (uint8_t )buffer[11] | (uint8_t)buffer[10] << 8;
    ms_log (0, "Checking Day of Year value: %d\n", doy);
    if (366 < doy || 1 > doy)
    {

        ms_log (2, "Error, Day Of Year value out of range (1-366)\n");
        header_valid = false;
    }
    /*  Dylans way
     * //uint16_t doy = buffer[10] + (buffer[11]*(256)
    */


    //Check valid hour range
    uint8_t hours = (uint8_t)buffer[12];
    ms_log (0, "Checking Hours value: %d\n", hours);
    if ( hours < 0 || hours > 23)
    {
        ms_log (2, "Error, Hours value out of range (0-23)\n");
        header_valid = false;
    }

    //Check valid min rage
    uint8_t mins = (uint8_t)buffer[13];
    ms_log (0, "Checking Mins value: %d\n", mins);
    if ( mins < 0 || mins > 59)
    {
        ms_log (2, "Error, Mins value out of range (0-59)\n");
        header_valid = false;
    }

    //Check valid seconds range
    uint8_t secs = (uint8_t)buffer[14];
    ms_log (0, "Checking Secs value: %d\n", secs);
    if ( secs < 0 || secs > 60 )
    {
        ms_log (2, "Error, Secs value out of range (1-366)\n");
        header_valid = false;
    }


    //Check Valid nanoseconds
    uint32_t nanoseconds = ((uint8_t)buffer[7] | ((uint8_t)buffer[6] << 8) | ((uint8_t)buffer[5] << 16) | ((uint8_t)buffer[4] << 24));
    ms_log (0, "Checking Nanoseconds value: %d\n", nanoseconds);
    if (999999999 < nanoseconds)
    {
        /*TODO to many nanoseconds */
    }

    //Check valid Payload Type
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
        case 14: /* Steim-2 integer compressin, big-endian */
            ms_log(0, "Steim-2 integer compression, big-endian\n");
            break;
            //TODO verify this with chad
        case 16: /* GEOSCOPE Muxed 16/4 bit gain/exp ,little endian(?) */
            ms_log(0, "GEOSCOPE Muxed 16/4 bit gain/exp, little endian(?)\n");
            break;
        case 19: /* Steim-3 integer compressin, big-endian */
            ms_log(0, "Steim-3 integer compression, big-endian\n");
            break;
            //TODO verify this with chad
        case 30: /* SRO Gain Ranged Format, ?-endian */
            ms_log(0, "SRO Gain Ranged Format, ?-endian\n");
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
            header_valid = false;
            break;
    };



    //Get Sample Rate
    double sample_rate;
    //TODO  for parsing buffer, in progress
    //buffer_to_number(buffer+16, sizeof(double), XSEED_DOUBLE, &sample_rate);
    //TODO need check for valid sample rate
    sample_rate = *((double*)((uint8_t*)buffer+16));
    if(sample_rate < 0 )
    {
        sample_rate = sample_rate * (-.01);//TODO ?????
    }
    ms_log(0, "Checking sample rate value: %f\n", sample_rate);

    //Get Number of Samples
    //TODO need check for valid number_samples
    //uint32_t number_samples_old = buffer[24] + buffer[25]*(0xFF+1) +buffer[26]*(0xFFFF+1) + buffer[27]*(0xFFFFFF+1);
    uint32_t number_samples = ((uint8_t)buffer[27] | ((uint8_t)buffer[26] << 8) | ((uint8_t)buffer[25] << 16) | ((uint8_t)buffer[24] << 24));
    ms_log(0, "Checking number of samples value: %d\n", number_samples);

    //Get CRC
    //uint32_t CRC = buffer[28] + buffer[29]*(0xFF+1) +buffer[30]*(0xFFFF+1) + buffer[31]*(0xFFFFFF+1);
    uint32_t CRC = ((uint8_t)buffer[31]) | ((uint8_t)buffer[30] << 8) | ((uint8_t)buffer[29] << 16) | ((uint8_t)buffer[28]) << 24 ;
    ms_log(0, "CRC value: 0x%0X\n",CRC);

    //Get dataPubVersion
    //TODO Check for valid dataPubVersion?
    uint8_t dataPubVersion = (uint8_t)buffer[32];
    ms_log(0, "Data Publication Version value: %d\n",dataPubVersion);
    uint8_t identifier_l = (uint8_t)buffer[33];
    ms_log(0, "Identifier Length value: %d\n",identifier_l);
    uint16_t extra_header_l = (uint8_t )buffer[35] | (uint8_t)buffer[34] << 8;
    ms_log(0, "Extra Header Length value: %d\n",extra_header_l);
    uint32_t payload_l = ((uint8_t)buffer[39] | ((uint8_t)buffer[38] << 8) | ((uint8_t)buffer[37] << 16) | ((uint8_t)buffer[36] << 24));
    ms_log(0, "Payload Length value: %d\n",payload_l);

    //assign to output values
    *payload_fmt = payload;
    *extra_header_len =  extra_header_l;
    *payload_len = payload_l;
    *identifier_len = identifier_l;


    return header_valid;

}
