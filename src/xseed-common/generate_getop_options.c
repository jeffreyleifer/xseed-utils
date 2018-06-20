#include <stdlib.h>
#include <string.h>
#include "cmd_opt.h"
#include "constants.h"

int expand_char (char **ptr, int current_len, size_t obj_size)
{
    int ret_len;

    if (0 ==  current_len)
    {
        ret_len = 1;
    }
    else
    {
        ret_len = (int)(1.5f * (double)current_len);
    }

    if (ret_len == current_len)
    {
        ret_len++;
    }

    *ptr = realloc(*ptr, ret_len * obj_size);
    if (NULL == *ptr)
    {
        return XSEED_MALLOC_ERROR;
    }
    return ret_len;
}

int xseed_get_short_getopt_string (char **short_opt_string, struct xseed_option_s *options)
{
    if (null == options)
    {
        return XSEED_BAD_INPUT;
    }
    char *temp=NULL;
    int total_len=0, alloc_len =0;
    for (struct xseed_option_s *option_ptr=options; option_ptr != NULL && 0 != option_ptr->short_option;option_ptr++)
    {
        if (option_ptr->short_option < 0 || option_ptr->argument_type < 0 || option_ptr->argument_type > 2)
        {
            continue;
        }
        int len;
        char *str;
        switch (option_ptr->argument_type)
        {
            case MANDATORY_OPTARG:
                len = 1;
                str = ":";
                break;
            case OPTIONAL_OPTARG:
                len = 2;
                str = "::";
                break;
            default:
                len = 0;
                str = "";
                break;
        }
        while (total_len + 2 + len > alloc_len)
        {
            alloc_len = expand_char(&temp, alloc_len, sizeof(char));
        }
        snprintf(temp + total_len, alloc_len - total_len, "%c%s", option_ptr->short_option, str);
        total_len += 1 + len;
    }
    *short_opt_string = temp;
}

int xseed_get_long_getopt_array(struct option **long_opt_array, struct xseed_option_s *options)
{
    if (NULL == options)
    {
        return XSEED_BAD_INPUT;
    }
    struct option *temp = NULL;
    int total_len, alloc_len = 0;

    for (struct xseed_option_s *option_ptr=options; option_ptr != NULL && 0 != option_ptr->short_option;option_ptr++)
    {
        if (option_ptr->long_option == NULL || option_ptr->argument_type < 0 || option_ptr->argument_type > 2)
        {
            continue;
        }
        while (total_len + 2 > alloc_len)
        {
            alloc_len = expand_char(&temp, alloc_len, sizeof(char));
        }
        (temp+total_len)->name = option_ptr->long_option;
        (temp+total_len)->has_arg = option_ptr->argument_type;
        (temp+total_len)->flag = option_ptr->var_to_fill;
        (temp+total_len)->val = option_ptr->short_option;
        total_len++;
    }
    memset(temp+total_len, 0, sizeof(struct option));
    *long_opt_array = temp;
    return total_len;
}
