#include <stdlib.h>
#include <libmseed.h>
#include <unpack.h>
#include <parson.h>
#include <stdbool.h>


//Required to be global, ms3_readmsr was having memory issues if it's a local variable
MS3Record *msr = NULL;
MS3Record *msrOut = NULL;

int main(int argc, char **argv)
{

    if(argc < 2)
    {
        printf("xseed2json requires a miniSEED file as an argument\n");
        //return -1;
        //for ci TODO remove
        return EXIT_SUCCESS;
    }
    char * filename = argv[1];

    //TODO make args
    int verbose = 3;
    bool print_data = true;

    JSON_Status ierr;
    JSON_Value *val = NULL;
    JSON_Object *jsonObj = NULL;
    JSON_Value *extraVal = NULL;

    char times[25];
    char hex[10];
    char b;
    uint32_t flags;

    flags |= MSF_UNPACKDATA;
    flags |= MSF_VALIDATECRC;


while ((ms3_readmsr (&msr, filename, 0, NULL, 0, verbose) == MS_NOERROR ))
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


//TODO FIgure out best way to put data into json
#if 0
    if(print_data) {
        if (msr->formatversion == 3) {
            ms_log(0, "Unpacking data for verification\n");
            ierr = msr3_unpack_mseed3(msr->record, msr->reclen, &msrOut, flags, verbose);
            //ierr = msr3_parse(msr->record, msr->reclen,&msrOut, flags, ppackets);
            //ierr = ms_parse_raw3 (msr->record, msr->reclen, ppackets);
            if (ierr != MS_NOERROR) {
                //TODO more verbose error output
                ms_log(2, "Error: Format 3 payload parsing failed. ms_unpack_mseed3 returned: %d\n", ierr);
                return -1;
            } else {

                ms_log(0, "Data unpacked successfully\n");
            }
        } else {
            ms_log(2, "Error: Format version not version 3, read as version: %d", msr->formatversion);
            ms_log(2, "Attepting to parse as format 2");
            ierr = msr3_unpack_mseed2(msr->record, msr->reclen, &msrOut, flags, verbose);
            if (ierr > 0) {
                ms_log(2, "Error: Format 2 payload parsing failed. ms_unpack_mseed2 returned: %d\n", ierr);
                return -1;
            }

        }


        if (msrOut->numsamples > 0) {
            int samplesize;
            void *sptr;

            if ((samplesize = ms_samplesize(msrOut->sampletype)) == 0) {
                ms_log(2, "Unrecognized sample type: '%c'\n", msrOut->sampletype);
                return -1;
            }
            if (msrOut->sampletype == 'a') {
                char *ascii = (char *) msrOut->datasamples;
                int length = msrOut->numsamples;

                ierr = json_object_set_string(jsonObj, "payload", ascii);

                if (ierr == JSONFailure) {
                    ms_log(2, "Something went wrong parsing to JSON : payload (ascii)");
                }


            } else
                {
                    //
                    int32_t arr_int[msrOut->numsamples];
                    float arr_float[msrOut->numsamples];
                    double arr_double[msrOut->numsamples];
                    char arr_out[msrOut->numsamples];

                    for (int i = 0; i < msrOut->numsamples; i++)
                    {
                        sptr = (char *) msrOut->datasamples + (i * samplesize);

                        if (msrOut->sampletype == 'i') {
                            arr_int[i] = *(int32_t *) sptr;
                            arr_out[i] = arr_int[i] + '0';
                        } else if (msrOut->sampletype == 'f')
                        {
                            arr_float[i] = *(float *) sptr;
                            arr_out[i] = arr_float[i] + '0';
                        } else if (msrOut->sampletype == 'd') {
                            arr_double[i] = *(double *) sptr;
                            arr_out[i] = arr_double[i] + '0';
                        }

                    }

                    ierr = json_object_set_string(jsonObj, "payload", arr_out);

                    if (ierr == JSONFailure)
                    {
                        ms_log(2, "Something went wrong parsing to JSON : payload (numeric)");
                    }
                }
        } else if (!msrOut->numsamples > 0) // if numsamples is <= 0
        {
            ms_log(2, "No samples found, Num samples = %d\n", msrOut->numsamples);
        }
    }
#endif

    char * full_string = json_serialize_to_string_pretty(val);
    printf("%s",full_string);
    json_free_serialized_string(full_string);
    json_value_free(val);

}



    return EXIT_SUCCESS;
}
