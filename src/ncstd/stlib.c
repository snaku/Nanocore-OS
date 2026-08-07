#include "ncstd/stdlib.h"
#include "ncstd/memory.h"
#include "ncstd/bool.h"

char* itoa(int value, char* dst)
{
    static char table[10] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };

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
        dst[i++] = table[value % 10];
        value /= 10;
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