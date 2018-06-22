#include <stdlib.h>
#include <string.h>
#include "xseed_string.h"
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
/*size_t strnlen(const char *s, size_t max_len)
{
    size_t len = 0;
    for (; len < max_len && 0 != s[len]; len);
    return len;
}*/

char *strndup(const char *s, size_t n)
{
    size_t actual_len = strnlen(s, n);
    char * out = (char *) calloc(actual_len + 1, sizeof(char));
    memcpy(out, s, actual_len);
    return out;
};
#endif
