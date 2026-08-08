#include "ncstd/string.h"
#include "ncstd/memory.h"

size_t strlen(const char* str)
{
    if (str == NULL)
    {
        return 0;
    }

    size_t len = 0;

    while (*str++ != '\0')
    {
        len++;
    }

    return len;
}

char* strcpy(char* dst, const char* src)
{
    return (char*)memcpy(dst, src, strlen(src) + 1);
}