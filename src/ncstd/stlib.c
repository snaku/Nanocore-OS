#include "ncstd/stdlib.h"
#include "ncstd/memory.h"
#include "ncstd/bool.h"
#include "ncstd/ctype.h"

char* itoa(int64_t value, char* dst, int base)
{
    static char table[16] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'
    };

    if (base < 2 ||
        base > 16)
    {
        return NULL;
    }

    if (dst == NULL)
    {
        return NULL;
    }

    int i = 0;
    ncbool isNegative = false;

    if (value == 0)
    {
        dst[0] = '0';
        dst[1] = '\0';

        return dst;
    }

    if (value < 0)
    {
        isNegative = true;
        value = -value; // to positive
    }

    while (value > 0)
    {
        dst[i++] = table[value % base];
        value /= base;
    }

    if (isNegative)
    {
        dst[i++] = '-';
    }

    dst[i] = '\0';

    // put numbers to the correct order
    int start = 0;
    int end = i - 1;
    while (start < end)
    {
        char tmp = dst[start];
        dst[start] = dst[end];
        dst[end] = tmp;

        start++;
        end--;
    }

    return dst;
}

uint64_t strtoull(const char* str, char** endPtr, int base)
{
    if (str == NULL)
    {
        if (endPtr != NULL)
        {
            *endPtr = (char*)str;
        }

        return 0;
    }

    if (base < 2 ||
        base > 16)
    {
        if (endPtr != NULL)
        {
            *endPtr = (char*)str;
        }

        return 0;
    }

    while (isspace(*str))
    {
        str++;
    }

    if (base == 16 &&
        str[0] == '0' &&
        (str[1] == 'x' || str[1] == 'X'))
    {
        str += 2;
    }

    const char* digitsStart = str;
    uint64_t val = 0;

    while (*str != '\0')
    {
        char c = *str;
        int digit;

        if (isdigit(c))
        {
            digit = c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            digit = c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'F')
        {
            digit = c - 'A' + 10;
        }
        else
        {
            break;
        }

        if (digit >= base)
        {
            break;
        }

        val = val * base + digit;
        str++;
    }

    if (endPtr != NULL)
    {
        *endPtr = (char*)str;
    }

    if (str == digitsStart)
    {
        return 0;
    }

    return val;
}