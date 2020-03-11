#include <stddef.h>
#include <stdint.h>

#include "info.h"
#include "bios.h"
#include "printk.h"

Info info;

int main(Info arg)
{
    info = arg;
    bios_screen_clear();
    printk("Hello InOS\n");

    bios_exit();

    return 0;
}
