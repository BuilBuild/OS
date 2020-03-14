#include <stddef.h>
#include <stdint.h>

#include "info.h"
#include "bios.h"
#include "printk.h"
#include "gdt.h"

Info info;

__attribute__((section(".begin"))) int main(Info *arg)
{
    info = *arg;

    //设置基础栈地址
    asm volatile(
        "mov $0xa0000, %%rsp\n\t"
        "mov %%rsp, %%rbp\n\t"
        :
        :

        :);

    bios_screen_clear();
    printk("Hello InOS\n");
    init_gdt();
    bios_exit();

    return 0;
}
