#include <stdlib.h>
#include <libmseed.h>
#include <parson.h>


MS3Record *msr;

int main(int argc, char **argv)
{

    JSON_Status ierr;

    JSON_Value *val = NULL;
    JSON_Object *jsonObj = NULL;
    JSON_Value *extraVal = NULL;
    char * filename = argv[1];
    char times[25];
    char hex[10];
    char b;
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




while ((ms3_readmsr (&msr, filename, 0, NULL, 0, 0) == MS_NOERROR ))
{

    if (!msr)
        return -1;

    val = json_value_init_object();
    jsonObj = json_value_get_object(val);

    /* Generate a start time string */
    ms_nstime2seedtimestr(msr->starttime, times, 1);


        ierr = json_object_set_string(jsonObj, "tsid", msr->tsid);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : tsID");
        }

        ierr = json_object_set_number(jsonObj, "pubversion", msr->pubversion);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : pubversion");
        }

        ierr = json_object_set_number(jsonObj, "reclen", msr->reclen);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : reclen");
        }

        ierr = json_object_set_number(jsonObj, "reclen", msr->formatversion);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : formatversion");
        }

        ierr = json_object_set_number(jsonObj, "reclen", msr->formatversion);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : formatversion");
        }

        ierr = json_object_set_string(jsonObj, "starttime", times);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : starttime");
        }

        ierr = json_object_set_number(jsonObj, "samplecnt", msr->samplecnt);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : formatversion");
        }

        ierr = json_object_set_number(jsonObj, "samprate", msr_sampratehz(msr));

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : formatversion");
        }


        ierr = json_object_set_number(jsonObj, "flags", msr->flags);
        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : flags");
        }

        sprintf(hex, "0x%0X", (unsigned int)msr->crc);
        ierr = json_object_set_string(jsonObj, "crc", hex);


        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : crc");
        }

        ierr = json_object_set_number(jsonObj, "extralength", msr->extralength);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : extralength");
        }

        ierr = json_object_set_number(jsonObj, "datalength", msr->datalength);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : datalength");
        }

        ierr = json_object_set_string(jsonObj, "encoding", (char *) ms_encodingstr(msr->encoding));

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : encoding");
        }
        ierr = json_object_set_number(jsonObj, "encodingval", msr->encoding);

        if(ierr == JSONFailure)
        {
            ms_log(2,"Something went wrong parsing to JSON : encodingval");
        }

        if(msr->extralength > 0 && msr->extra)
        {
            extraVal = json_parse_string(msr->extra);
            ierr = json_object_set_value(jsonObj, "extra", extraVal);

            if (ierr == JSONFailure)
            {
                ms_log(2, "Something went wrong parsing to JSON : extra");
            }

        }


    char * full_string = json_serialize_to_string_pretty(val);
    printf("%s",full_string);
    json_free_serialized_string(full_string);
    json_value_free(val);


}
    return EXIT_SUCCESS;
}
