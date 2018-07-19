#include <stdlib.h>
#include <libmseed.h>
#include <unpack.h>
#include <parson.h>
#include <stdbool.h>
#include <xseed-common/cmd_opt.h>
#include <xseed-common/files.h>
#include <xseed-common/xseed_string.h>
#include <libmseed.h>
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <xseed-common/vcs_getopt.h>
#else
#include <unistd.h>
#include <getopt.h>
#endif


//CMD line option structure
static const struct xseed_option_s args[] = {
        {'h',    "help", "Usage", NULL, NO_OPTARG},
        {'f',    "file", "File to validate", NULL, MANDATORY_OPTARG},
        {'v', "verbose", "Verbosity level", NULL, OPTIONAL_OPTARG},
        {  0,         0, 0, 0, 0}};


//Required to be global, ms3_readmsr having memory issues if it's a local variable
MS3Record *msrOut = NULL;

int main(int argc, char **argv)
{



    MS3Record *msr = NULL;

    JSON_Status ierr;
    JSON_Value *val = NULL;
    JSON_Object *jsonObj = NULL;
    JSON_Value *extraVal = NULL;
    JSON_Array *payload_arr = NULL;

    char times[25];
    char hex[10];
    char b;
    uint32_t flags;

    //vars to store command line options/args
    char *short_opt_string = NULL;
    struct option *long_opt_array = NULL;
    int opt;
    int longindex;
    unsigned char display_usage = 0;
    uint8_t verbose = 0;
    bool print_data = false;
    char *file_name = NULL;

    //parse command line args
    xseed_get_short_getopt_string (&short_opt_string, args);
    xseed_get_long_getopt_array(&long_opt_array, args);

    //Set flag to unpack data and check CRC
    flags |= MSF_UNPACKDATA;
    flags |= MSF_VALIDATECRC;

    while(-1 != (opt=getopt_long(argc, argv, short_opt_string, long_opt_array, &longindex)))
    {
        switch(opt)
        {
            //int file_name_size;
            case 'f':
                file_name = strndup(optarg, 1024);
                break;
            case 'v':
                if (0 == optarg )
                {
                    verbose++;
                }
                else
                {
                    verbose = strlen(optarg) + 1;
                }
                break;
            case 'h':
                display_usage =1;
                break;
            default:
                display_usage++;
                break;
        }
        if (display_usage > 0)
        {
            break;
        }
    }
    if (display_usage > 0 || NULL == file_name)
    {
        display_help(argv[0], "Program to Print a miniSEED file in JSON format", args);
        return display_usage < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    if (!xseed_file_exists(file_name))
    {
        printf("Error reading file: %s, File Not Found! \n",file_name);
        return EXIT_FAILURE;
    }


while ((ms3_readmsr (&msr, file_name, 0, NULL, 0, verbose) == MS_NOERROR ))
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


        if(verbose > 0)
        {
            print_data = true;
        }

    if(print_data) {
        if (msr->formatversion == 3) {
            ms_log(0, "Unpacking data for verification\n");
            ierr = msr3_unpack_mseed3(msr->record, msr->reclen, &msrOut, flags, verbose);
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

                    json_object_set_value(jsonObj, "Payload", json_value_init_array());
                    payload_arr = json_object_get_array(jsonObj, "Payload");

                    for (int i = 0; i < msrOut->numsamples; i++)
                    {
                        sptr = (char *) msrOut->datasamples + (i * samplesize);

                        if (msrOut->sampletype == 'i')
                        {
                            json_array_append_number(payload_arr,*(int32_t *) sptr);
                        } else if (msrOut->sampletype == 'f')
                        {
                            json_array_append_number(payload_arr,*(float *) sptr);
                        } else if (msrOut->sampletype == 'd')
                        {
                            json_array_append_number(payload_arr,*(double *) sptr);
                        }

                    }

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


    char * full_string = json_serialize_to_string_pretty(val);
    printf("%s",full_string);
    json_free_serialized_string(full_string);
    json_array_clear(payload_arr);
    json_value_free(val);



}



    return EXIT_SUCCESS;
}
