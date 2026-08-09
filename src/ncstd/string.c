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

int strcmp(const char* str1, const char* str2)
{
    size_t i = 0;

    while (str1[i] == str2[i])
    {
        if (str1[i] == '\0')
        {
            return 0;
        }

        i++;
    }

    return str1[i] - str2[i];
}

int strncmp(const char* str1, const char* str2, size_t len)
{
    size_t i = 0;

    while (i < len &&
           str1[i] == str2[i])
    {
        if (str1[i] == '\0')
        {
            return 0;
        }

        i++;
    }

    if (i == len)
    {
        return 0;
    }

    return str1[i] - str2[i];
}