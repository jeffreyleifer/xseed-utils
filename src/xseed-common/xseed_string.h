#include <stdlib.h>
#include <string.h>

//this is so these functions are availible to windows
#ifndef HAS_STRNLEN
extern size_t strnlen(const char *, size_t);
#endif
#ifndef HAS_STRNDUP
extern char *strndup(const char *, size_t);
#endif
