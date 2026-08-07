#include "ncstd/string.h"
#include "ncstd/memory.h"

int strlen(const char* str)
{
    if (str == NULL)
    {
        return 0;
    }

    int len = 0;

    while (*str++ != '\0')
    {
        len++;
    }

    return len;
}