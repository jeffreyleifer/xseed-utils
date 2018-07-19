#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libmseed.h>
#include "validator.h"
#include "warnings.h"

#include <errno.h>
#include <string.h>
#include <time.h>

#include <libmseed.h>
#include <unpack.h>
#include <mseedformat.h>


bool check_payloads(struct warn_options_s *options, FILE *input, uint32_t payload_len, uint8_t payload_fmt, char* file_name,uint8_t verbose)
{

    //Return value, initial state assumes valid payload
    bool answer = true;

    //TODO incomplete, solution to verify data payload without libmseed functions
    char *buffer = (char *) calloc(payload_len +1, sizeof(char));
    if (payload_len > fread(buffer, sizeof(char), payload_len, input))
    {
        ms_log(2,"Error: failed to read record data payload into buffer\n");
        answer = false;
    }
#if 0

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
#endif
    if (buffer)
    {
        free(buffer);
    }
    //End of incomplete solution, see prev TODO


    //Solution using libmseed builtin functions to

    bool print_data = false;
    if(verbose > 2)
    {
       print_data = true;
    }


    //Decode and check using libmseed's functions
    MS3Record *msr = NULL;
    MS3Record *msrOut = NULL;

    int retcode;
    int ierr;
    uint32_t flags;

    //unpack data payload, check CRC
    flags |= MSF_UNPACKDATA;
    flags |= MSF_VALIDATECRC;

    while ((ms3_readmsr (&msr, file_name, 0, NULL, 0, verbose) == MS_NOERROR ))
    {
         //msr3_print (msr, verbose);

        //TODO get status message
        if (msr->formatversion == 3)
        {
            ms_log (0, "Unpacking data for verification\n");
            ierr = msr3_unpack_mseed3(msr->record, msr->reclen,&msrOut, flags, verbose);
            //ierr = msr3_parse(msr->record, msr->reclen,&msrOut, flags, ppackets);
            //ierr = ms_parse_raw3 (msr->record, msr->reclen, ppackets);
            if(ierr != MS_NOERROR)
            {
                //TODO more verbose error output
                ms_log(2,"Error: Format 3 payload parsing failed. ms_unpack_mseed3 returned: %d\n", ierr);
                answer = false;
            } else
            {
                ms_log(0,"Data unpacked successfully\n");
            }
        }
        else
        {
            ms_log(2,"Error: Format version not version 3, read as version: %d",msr->formatversion);
            ms_log(2,"Attepting to parse as format 2");
            ierr = msr3_unpack_mseed2 (msr->record, msr->reclen,&msrOut, flags, verbose);
            if(ierr > 0)
            {
                ms_log(2,"Error: Format 2 payload parsing failed. ms_unpack_mseed2 returned: %d\n", ierr);
                answer = false;
            }

        }



        if (msrOut->numsamples > 0 && print_data)
        {
            int line, col, cnt, samplesize;
            int lines = (msrOut->numsamples / 6) + 1;
            void *sptr;

            if ((samplesize = ms_samplesize (msrOut->sampletype)) == 0)
            {
                ms_log (2, "Unrecognized sample type: '%c'\n", msrOut->sampletype);
                answer = false;
            }
            if (msrOut->sampletype == 'a')
            {
                char *ascii = (char *)msrOut->datasamples;
                int length  = msrOut->numsamples;

                ms_log (0, "ASCII Data:\n");

                /* Print maximum log message segments */
                while (length > (MAX_LOG_MSG_LENGTH - 1))
                {
                    ms_log (0, "%.*s", (MAX_LOG_MSG_LENGTH - 1), ascii);
                    ascii += MAX_LOG_MSG_LENGTH - 1;
                    length -= MAX_LOG_MSG_LENGTH - 1;
                }

                /* Print any remaining ASCII and add a newline */
                if (length > 0)
                {
                    ms_log (0, "%.*s\n", length, ascii);
                }
                else
                {
                    ms_log (0, "\n");
                }
            }
            else
            {
                for (cnt = 0, line = 0; line < lines; line++) {
                    for (col = 0; col < 6; col++) {
                        if (cnt < msrOut->numsamples) {
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
        } else if(!msrOut->numsamples > 0) // if numsamples is <= 0 // if numsamples is <= 0
        {
           ms_log(0,"No samples found, Num samples = %d\n", msrOut->numsamples);
        }
    }


    return answer;

}

#if 0

/***************************************************************************
 * lmtestparse.c
 *
 * A program for libmseed parsing tests.
 *
 * Written by Chad Trabant, IRIS Data Management Center
 ***************************************************************************/


static int testFile (char* file_name,int argc, char **argv);
static int parameter_proc (int argcount, char **argvec);
static void print_stderr (char *message);
static void usage (void);

/* Binary I/O for Windows platforms */
#ifdef LMP_WIN
unsigned int _CRT_fmode = _O_BINARY;
#endif

int testFile (char* file_name,int argc, char **argv)
{

    static flag verbose    = 3;
    static flag ppackets   = 1;
    static flag basicsum   = 1;
    static flag tracegap   = 0;
    static int printraw    = 0;
    static int printdata   = 0;
    static int reclen      = -1;
    static char *inputfile = 0;

    static double timetol     = -1.0; /* Time tolerance for continuous traces */
    static double sampratetol = -1.0; /* Sample rate tolerance for continuous traces */
    MS3TraceList *mstl = 0;
    MS3Record *msr = 0;
    uint32_t flags = 0;

    int64_t totalrecs = 0;
    int64_t totalsamps = 0;
    int retcode;

    /* Redirect libmseed logging facility to stderr for consistency */
    //ms_loginit (print_stderr, NULL, print_stderr, NULL);

    /* Process given parameters (command line and parameter file) */
//    if (parameter_proc (argc, argv) < 0)
//        return -1;

    inputfile = file_name;

    /* Validate CRC and unpack data samples */
    flags |= MSF_VALIDATECRC;

    //if (printdata)
        flags |= MSF_UNPACKDATA;

    //if (tracegap)
        mstl = mstl3_init (NULL);

    /* Loop over the input file */
    while (ms3_readmsr (&msr, inputfile, NULL, NULL, flags, verbose))
    {
        totalrecs++;
        totalsamps += msr->samplecnt;

        if (tracegap)
        {
            mstl3_addmsr (mstl, msr, 0, 1, timetol, sampratetol);
        }
        else
        {
            if ( printraw )
            {
                if (msr->formatversion == 3)
                    ms_parse_raw3 (msr->record, msr->reclen, ppackets);
                else
                    ms_parse_raw2 (msr->record, msr->reclen, ppackets, -1);
            }
            else
            {
                msr3_print (msr, ppackets);
            }

            if (printdata && msr->numsamples > 0)
            {
                int line, col, cnt, samplesize;
                int lines = (msr->numsamples / 6) + 1;
                void *sptr;

                if ((samplesize = ms_samplesize (msr->sampletype)) == 0)
                {
                    ms_log (2, "Unrecognized sample type: '%c'\n", msr->sampletype);
                }
                if (msr->sampletype == 'a')
                {
                    char *ascii = (char *)msr->datasamples;
                    int length  = msr->numsamples;

                    ms_log (0, "ASCII Data:\n");

                    /* Print maximum log message segments */
                    while (length > (MAX_LOG_MSG_LENGTH - 1))
                    {
                        ms_log (0, "%.*s", (MAX_LOG_MSG_LENGTH - 1), ascii);
                        ascii += MAX_LOG_MSG_LENGTH - 1;
                        length -= MAX_LOG_MSG_LENGTH - 1;
                    }

                    /* Print any remaining ASCII and add a newline */
                    if (length > 0)
                    {
                        ms_log (0, "%.*s\n", length, ascii);
                    }
                    else
                    {
                        ms_log (0, "\n");
                    }
                }
                else
                    for (cnt = 0, line = 0; line < lines; line++)
                    {
                        for (col = 0; col < 6; col++)
                        {
                            if (cnt < msr->numsamples)
                            {
                                sptr = (char *)msr->datasamples + (cnt * samplesize);

                                if (msr->sampletype == 'i')
                                    ms_log (0, "%10d  ", *(int32_t *)sptr);

                                else if (msr->sampletype == 'f')
                                    ms_log (0, "%10.8g  ", *(float *)sptr);

                                else if (msr->sampletype == 'd')
                                    ms_log (0, "%10.10g  ", *(double *)sptr);

                                cnt++;
                            }
                        }
                        ms_log (0, "\n");

                        /* If only printing the first 6 samples break out here */
                        if (printdata == 1)
                            break;
                    }
            }
        }
    }

    if (retcode != MS_ENDOFFILE)
        ms_log (2, "Cannot read %s: %s\n", inputfile, ms_errorstr (retcode));

    if (tracegap)
        mstl3_printtracelist (mstl, 0, 1, 1);

    /* Make sure everything is cleaned up */
    ms3_readmsr (&msr, NULL, NULL, NULL, flags, 0);

    if (mstl)
        mstl3_free (&mstl, 0);

    if (basicsum)
        ms_log (1, "Records: %" PRId64 ", Samples: %" PRId64 "\n",
                totalrecs, totalsamps);

    return 0;
} /* End of main() */

/***************************************************************************
 * parameter_proc():
 * Process the command line parameters.
 *
 * Returns 0 on success, and -1 on failure
 ***************************************************************************/
static int
parameter_proc (int argcount, char **argvec)
{
#if 0
    int optind;

    /* Process all command line arguments */
    for (optind = 1; optind < argcount; optind++)
    {
        if (strcmp (argvec[optind], "-V") == 0)
        {
//            ms_log (1, "%s version: %s\n", PACKAGE, VERSION);
            exit (0);
        }
        else if (strcmp (argvec[optind], "-h") == 0)
        {
            usage ();
            exit (0);
        }
        else if (strncmp (argvec[optind], "-v", 2) == 0)
        {
  //          verbose += strspn (&argvec[optind][1], "v");
        }
        else if (strncmp (argvec[optind], "-p", 2) == 0)
        {
    //        ppackets += strspn (&argvec[optind][1], "p");
        }
        else if (strncmp (argvec[optind], "-z", 2) == 0)
        {
      //      printraw = 1;
        }
        else if (strncmp (argvec[optind], "-d", 2) == 0)
        {
    //        printdata = 1;
        }
        else if (strncmp (argvec[optind], "-D", 2) == 0)
        {
   //         printdata = 2;
        }
        else if (strncmp (argvec[optind], "-tg", 3) == 0)
        {
   //         tracegap = 1;
        }
        else if (strcmp (argvec[optind], "-s") == 0)
        {
   //         basicsum = 1;
        }
        else if (strcmp (argvec[optind], "-r") == 0)
        {
            reclen = atoi (argvec[++optind]);
        }
        else if (strncmp (argvec[optind], "-", 1) == 0 &&
                 strlen (argvec[optind]) > 1)
        {
            ms_log (2, "Unknown option: %s\n", argvec[optind]);
            exit (1);
        }
        else if (inputfile == 0)
        {
            inputfile = argvec[optind];
        }
        else
        {
            ms_log (2, "Unknown option: %s\n", argvec[optind]);
            exit (1);
        }
    }

    /* Make sure an inputfile was specified */
    if (!inputfile)
    {
        ms_log (2, "No input file was specified\n\n");
        ms_log (1, "%s version %s\n\n", PACKAGE, VERSION);
        ms_log (1, "Try %s -h for usage\n", PACKAGE);
        exit (1);
    }

    /* Report the program version */
    if (verbose)
        ms_log (1, "%s version: %s\n", PACKAGE, VERSION);
#endif
    return 0;
} /* End of parameter_proc() */

/***************************************************************************
 * print_stderr():
 * Print messsage to stderr.
 ***************************************************************************/
static void
print_stderr (char *message)
{
    fprintf (stderr, "%s", message);
} /* End of print_stderr() */

#endif