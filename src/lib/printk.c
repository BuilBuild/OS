#include "printk.h"
#include "stdlib.h"

#include "bios.h"

#include <stdarg.h> //可变参数使用所需头文件
#include <stddef.h>

void printk(const char *fmt, ...)
{
    va_list valist;
    va_start(valist, fmt);

    char buf[32];
    size_t i = 0;
    while (1)
    {
        while (i < 32 && *fmt != '%' && *fmt != '\0')
        {
            buf[i] = *fmt++;
            ++i;
        }

        buf[i] = '\0';
        bios_puts(buf, WHITE, BLACK);

        if (*fmt == '\0')
        {
            break;
        }
        else
        {
            ++fmt;
            if (*fmt == 's')
            {
                bios_puts(va_arg(valist, const char *), WHITE, BLACK);
                ++fmt;
                continue;
            }
            else if (*fmt == 'c')
            {
                buf[0] = va_arg(valist, int);
                i = 1;
                ++fmt;
                continue;
            }
            else if (*fmt == 'l')
            {
                itoa((unsigned long)va_arg(valist, long), buf, *++fmt);
            }
            else
            {
                itoa((unsigned long)va_arg(valist, int), buf, *fmt);
            }
            ++fmt;
            i = 0;
            bios_puts(buf, WHITE, BLACK);
        }
    }
    va_end(valist);
}
