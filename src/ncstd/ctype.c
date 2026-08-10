#include "ncstd/ctype.h"

ncbool isalpha(char c)
{
    return islower(c) || isupper(c);
}

ncbool islower(char c)
{
    return c >= 'a' && c <= 'z';
}

ncbool isupper(char c)
{
    return c >= 'A' && c <= 'Z';
}

ncbool isspace(char c)
{
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r';
}

ncbool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

char toupper(char c)
{
    if (!isalpha(c) &&
        !islower(c))
    {
        return c;
    }

    return c - 0x20;
}