#include <stdlib.h>
#include <stdbool.h>
#include <xseed-common/array.h>
#include "warnings.h"
#include <xseed-common/xseed_string.h>


bool parse_warn_options(struct warn_options_s *warn_options, char * string_parse)
{
    char **tokened = NULL;
    size_t tokened_len = 0;
    size_t tokened_alloc = 0;
    bool bad_option = false;
    //split on comma
    for (char *token = strtok(string_parse, ","); token != NULL; token = strtok(NULL, ","))
    {
        while (tokened_alloc <= tokened_len)
        {
            tokened_alloc = expand_array((void **) &tokened, tokened_alloc, sizeof(char *));
        }
        int token_len = strnlen(token, 1024);
        tokened[tokened_len++] = strndup(token, token_len);
    }
    for (size_t i = 0; i < tokened_len && !bad_option;i++)
    {
        //split on equals
        char *flag = strtok(tokened[i],"=");
        char *value = strtok(NULL,"=");
        //trim front and back whitespaces
        //set values
        if (0 == strncmp("error", flag, strlen("error")))
        {
            warn_options->treat_as_errors = true;
        }
        else
        {
            bad_option = true;
        }
    }

    for (size_t i = 0; i < tokened_len;i++)
    {
        free(tokened[i]);
        tokened[i]=NULL;
    }
    free(tokened);
    tokened=NULL;
    return !bad_option;
}
