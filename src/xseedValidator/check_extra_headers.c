#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "validator.h"
#include "warnings.h"
#include <wjelement.h>
#include <libmseed.h>

#define SCHEMA_BUFFER_SIZE 1024u

static void schema_error_func(void *client, const char *format, ...)
{
//TODO format to other log messages
    va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}
/*TODO future improvement pass back stuff from extra_headers to validate payloads*/
bool check_extra_headers(struct warn_options_s *options, char *schema, FILE *input, uint16_t extra_header_len)
{
    char *buffer = (char *) calloc(extra_header_len + 1, sizeof(char));
    bool output = true;

    if ( extra_header_len > fread(buffer, sizeof(char), extra_header_len, input))
    {
        /* TODO warn reached end of file */
        free(buffer);
        buffer = NULL;
        output = false;
    }

    if (schema)
    {
        ms_log(0,"---Starting extra header verification---\n", schema);
        char schema_buffer[SCHEMA_BUFFER_SIZE];
        //* open schema
        ms_log(0,"Checking JSON file for valid structure: %s\n", schema);
        WJElement document_element = WJEParse(buffer);

        if(document_element != NULL)
        {
            //Print header to validate
            char *extraHeaderStr = WJEToString(document_element, true);
            ms_log(0, "Extra header output:\n%s\n\n", extraHeaderStr);
            free(extraHeaderStr);


            FILE *schema_file = fopen(schema, "r");
            WJElement schema_element = WJEOpenDocument(
                    WJROpenFILEDocument(schema_file, schema_buffer, SCHEMA_BUFFER_SIZE), NULL, NULL, NULL);
            WJEErrCB errFunc = &schema_error_func;
            /* check is valid json based on schema*/
            if (!WJESchemaValidate(schema_element, document_element, errFunc, NULL, NULL, NULL)) {
                output = false;
                /*TODO warn anything not printed by schema error */
                ms_log(3, "\nError: Schema not valid! Something went wrong in validation\n");

            } else {
                ms_log(0, "\nSchema Valid!\n");
            }

        } else
            {
                ms_log(1, "\nWarning: No extra headers found, nothing to validate schema against\n");
            }

    }
    else
    {
        /*TODO might want to warn that not checking extra headers because no schema loaded */
    }

    /*TODO other checks */

    if (buffer)
    {
        free (buffer);
    }
    ms_log(0,"---Finished extra header verification---\n", schema);
    return output;
}
