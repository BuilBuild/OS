#include "stdlib.h"

char *itoa(unsigned long val, char *buf, char radix)
{

    char *p = buf;
    char temp;
    unsigned digval;

    char a = 'a';
    if (radix == 'x')
    {
        radix = 16;
        a = 'a';
    }
    else if (radix == 'X')
    {
        radix = 16;
        a = 'A';
    }
    else if (radix == 'd')
    {
        radix = 10;
        if (*(long *)&val < 0)
        {
            *p++ = '-';
            val = -(long)val;
        }
    }
    else
    {
        radix = 10;
    }

    char *firstdig = p;

    do
    {
        digval = (unsigned)(val % radix);
        val /= radix;

        if (digval > 9)
            *p++ = (char)(digval - 10 + a);
        else
            *p++ = (char)(digval + '0');
    } while (val > 0);

    *p-- = '\0';

    do
    {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;
    } while (firstdig < p);

    return buf;
}