#include <stdlib.h>
#include <libmseed.h>
#include <parson.h>

int main(int argc, char **argv)
{



/***************************************************************************
 * msr3_print:
 *
 * Prints header values in an MS3Record struct.
 *
 * The value of details functions as follows:
 *  0 = print a single summary line
 *  1 = print most details of header
 * >1 = print all details of header and extra headers if present
 *
 ***************************************************************************/

//TODO use these for conversion
/*
    JSON_Status json_object_set_value(JSON_Object *object, const char *name, JSON_Value *value);
    JSON_Status json_object_set_string(JSON_Object *object, const char *name, const char *string);
    JSON_Status json_object_set_number(JSON_Object *object, const char *name, double number);
    JSON_Status json_object_set_boolean(JSON_Object *object, const char *name, int boolean);
*/

    MS3Record *msr;
    int8_t details;
    JSON_Status ierr;

    JSON_Object * jsonObj;
    char * filename = argv[1];

while ((ms3_readmsr (&msr, filename, 0, NULL, 0, 3) == MS_NOERROR ))
{

    char time[25];
    char b;

    if (!msr)
        return -1;
    JSON_Value *val = NULL;
    val = json_value_init_object();
    jsonObj = json_value_get_object(val);

    /* Generate a start time string */
    ms_nstime2seedtimestr(msr->starttime, time, 1);

    ierr = json_object_set_string(jsonObj, "StartTime", time);
    //json_object_set_number(jsonObj, "", double number);
    printf("Root element:\n  %s",json_serialize_to_string_pretty(val));
    /* Report information in the fixed header */
    if (details > 0) {
        ms_log(0, "%s, %d, %d (format: %d)\n",
               msr->tsid, msr->pubversion, msr->reclen, msr->formatversion);
        ms_log(0, "             start time: %s\n", time);
        ms_log(0, "      number of samples: %d\n", msr->samplecnt);
        ms_log(0, "       sample rate (Hz): %.10g\n", msr_sampratehz(msr));

        if (details > 1) {
            b = msr->flags;
            ms_log(0, "                  flags: [%u%u%u%u%u%u%u%u] 8 bits\n",
                   bit (b, 0x01), bit (b, 0x02), bit (b, 0x04), bit (b, 0x08),
                   bit (b, 0x10), bit (b, 0x20), bit (b, 0x40), bit (b, 0x80));
            if (b & 0x01)
                ms_log(0, "                         [Bit 0] Calibration signals present\n");
            if (b & 0x02)
                ms_log(0, "                         [Bit 1] Time tag is questionable\n");
            if (b & 0x04)
                ms_log(0, "                         [Bit 2] Clock locked\n");
            if (b & 0x08)
                ms_log(0, "                         [Bit 3] Undefined bit set\n");
            if (b & 0x10)
                ms_log(0, "                         [Bit 4] Undefined bit set\n");
            if (b & 0x20)
                ms_log(0, "                         [Bit 5] Undefined bit set\n");
            if (b & 0x40)
                ms_log(0, "                         [Bit 6] Undefined bit set\n");
            if (b & 0x80)
                ms_log(0, "                         [Bit 7] Undefined bit set\n");
        }

        ms_log(0, "                    CRC: 0x%0X\n", msr->crc);
        ms_log(0, "    extra header length: %d bytes\n", msr->extralength);
        ms_log(0, "    data payload length: %d bytes\n", msr->datalength);
        ms_log(0, "       payload encoding: %s (val: %d)\n",
               (char *) ms_encodingstr(msr->encoding), msr->encoding);

        if (details > 1 && msr->extralength > 0 && msr->extra) {
            ms_log(0, "          extra headers:\n");
            mseh_print(msr, 16);
        }
    } else {
        ms_log(0, "%s, %d, %d, %" PRId64 " samples, %-.10g Hz, %s\n",
               msr->tsid, msr->pubversion, msr->reclen,
               msr->samplecnt, msr->samprate, time);
    }
    /* End of msr3_print() */

}
    return EXIT_SUCCESS;
}
