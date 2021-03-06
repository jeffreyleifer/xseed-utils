#include <stdlib.h>
#include <libmseed.h>
#include <unpack.h>
#include <stdint.h>
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
        {'h',    "help h", "Usage", NULL, NO_OPTARG},
        {'f',    "file f", "File to validate", NULL, MANDATORY_OPTARG},
        {'v', "verbose v", "Verbosity level", NULL, OPTIONAL_OPTARG},
        {  0,         0, 0, 0, 0}};



//Required to be global, ms3_readmsr was having memory issues if it's a local variable
MS3Record *msrOut = NULL;

/*! @brief Prints xSEED file contains in human readable format
 *
 *  @param[in] argv[1] xSEED file to print
 *  @TODO add run flags for verbosity and print payload option
 *
 */
int main(int argc, char **argv)
{

    //vars for parsing xSEED file and payload
    MS3Record *msr = NULL;
    int retcode;
    int ierr;
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
        display_help(argv[0], "Program to Print xSEED file in human readable format\n", args);
        return display_usage < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    if (!xseed_file_exists(file_name))
    {
        printf("Error reading file: %s, File Not Found! \n",file_name);
        return EXIT_FAILURE;
    }


    if(verbose > 2)
    {
        print_data = true;
    }

    //Read in records
    while ((ms3_readmsr(&msr, file_name, 0, NULL, 0, verbose + 1) == MS_NOERROR))
    {
        //Print header info
        msr3_print(msr, verbose + 1);

        //The following code adapted from libmseed - msr3_parse(..)
        //Print payload if enabled
        if (print_data)
        {

            //If mimiSEEDv3
            if (msr->formatversion == 3)
            {
                ms_log(0, "Unpacking data for verification\n");
                ierr = msr3_unpack_mseed3(msr->record, msr->reclen, &msrOut, flags, verbose);
                //ierr = msr3_parse(msr->record, msr->reclen,&msrOut, flags, verbose);
                //ierr = ms_parse_raw3 (msr->record, msr->reclen, ppackets);
                if (ierr != MS_NOERROR)
                {
                    //TODO more verbose error output
                    ms_log(2, "Error: Format 3 payload parsing failed. ms_unpack_mseed3 returned: %d\n", ierr);
                    return -1;
                } else
                {
                    ms_log(0, "Data unpacked successfully\n");
                }
            } else //If older miniSEED format
            {
                ms_log(2, "Error: Format version not version 3, read as version: %d\n", msr->formatversion);
                ms_log(2, "Attepting to parse as format 2");
                ierr = msr3_unpack_mseed2(msr->record, msr->reclen, &msrOut, flags, verbose);
                if (ierr > 0)
                {
                    ms_log(2, "Error: Format 2 payload parsing failed. ms_unpack_mseed2 returned: %d\n", ierr);
                    return -1;
                }

            }

            //only attempt to print if data exists
            if (msrOut->numsamples > 0)
            {
                int line, col, cnt, samplesize;
                int lines = (msrOut->numsamples / 6) + 1;
                void *sptr;

                if ((samplesize = ms_samplesize(msrOut->sampletype)) == 0)
                {
                    ms_log(2, "Unrecognized sample type: '%c'\n", msrOut->sampletype);
                    return -1;
                }
                if (msrOut->sampletype == 'a')
                {
                    char *ascii = (char *) msrOut->datasamples;
                    int length = msrOut->numsamples;

                    ms_log(0, "ASCII Data:\n");

                    /* Print maximum log message segments */
                    while (length > (MAX_LOG_MSG_LENGTH - 1))
                    {
                        ms_log(0, "%.*s", (MAX_LOG_MSG_LENGTH - 1), ascii);
                        ascii += MAX_LOG_MSG_LENGTH - 1;
                        length -= MAX_LOG_MSG_LENGTH - 1;
                    }

                    /* Print any remaining ASCII and add a newline */
                    if (length > 0)
                    {
                        ms_log(0, "%.*s\n", length, ascii);
                    } else
                    {
                        ms_log(0, "\n");
                    }
                    //if samples are a number
                } else
                {
                    for (cnt = 0, line = 0; line < lines; line++)
                    {
                        for (col = 0; col < 6; col++)
                        {
                            if (cnt < msrOut->numsamples)
                            {
                                sptr = (char *) msrOut->datasamples + (cnt * samplesize);

                                if (msrOut->sampletype == 'i')
                                    ms_log(0, "%10d  ", *(int32_t *) sptr);

                                else if (msrOut->sampletype == 'f')
                                    ms_log(0, "%10.8g  ", *(float *) sptr);

                                else if (msrOut->sampletype == 'd')
                                    ms_log(0, "%10.10g  ", *(double *) sptr);

                                cnt++;
                            }
                        }
                        ms_log(0, "\n");
                    }
                }
            } else if (!msrOut->numsamples > 0) // if numsamples is <= 0
            {
                ms_log(0, "Print Payload error: No samples found, Num samples = %d\n", msrOut->numsamples);
            }
        }
    }

    return EXIT_SUCCESS;
}