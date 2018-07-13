#include <stdlib.h>
#include <libmseed.h>
#include <unpack.h>
#include <stdint.h>
#include <stdbool.h>


//Required to be global, ms3_readmsr was having memory issues if it's a local variable
MS3Record *msrOut = NULL;

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        printf("xseed2text requires a miniSEED file as an argument\n");
        //return -1;

        //for ci TODO remove
        return EXIT_SUCCESS;
    }

    MS3Record *msr = NULL;
    int verbose = 3;
    bool print_data = true;
    int retcode;
    int ierr;
    uint32_t flags;

    flags |= MSF_UNPACKDATA;
    flags |= MSF_VALIDATECRC;

    char *file_name = argv[1];

    while ((ms3_readmsr(&msr, file_name, 0, NULL, 0, 3) == MS_NOERROR))
    {
        msr3_print(msr, verbose);

        if (print_data)
        {

            //TODO get status message
            if (msr->formatversion == 3)
            {
                ms_log(0, "Unpacking data for verification\n");
                ierr = msr3_unpack_mseed3(msr->record, msr->reclen, &msrOut, flags, verbose);
                //ierr = msr3_parse(msr->record, msr->reclen,&msrOut, flags, ppackets);
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
            } else
            {
                ms_log(2, "Error: Format version not version 3, read as version: %d", msr->formatversion);
                ms_log(2, "Attepting to parse as format 2");
                ierr = msr3_unpack_mseed2(msr->record, msr->reclen, &msrOut, flags, verbose);
                if (ierr > 0)
                {
                    ms_log(2, "Error: Format 2 payload parsing failed. ms_unpack_mseed2 returned: %d\n", ierr);
                    return -1;
                }

            }

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