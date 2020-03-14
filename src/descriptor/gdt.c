#include <stddef.h>
#include <stdint.h>

#include "gdt.h"

static const uint64_t GDT[5] = {0,
                                0x0020980000000000, 0x0000920000000000,  //64位 内核 代码 和 数据段
                                0x0020f80000000000, 0x0000f20000000000}; //64位 用户 代码 和 数据段

void init_gdt()
{
    uint16_t ptr[5];
    ptr[0] = sizeof(GDT);
    *((const uint64_t **)&ptr[1]) = GDT;

    asm volatile(
        "lgdt (%%rax)\n\t"
        :
        : "a"(ptr)
        :);

    set_gdt(0x8, 0x10);
}

void set_gdt(uint16_t code, uint16_t data)
{
    asm volatile(
        "push %%rax\n\t"
        "lea __set_gdt_data, %%rcx\n\t"
        "push %%rcx\n\t"
        "lretq\n\t"

        "__set_gdt_data:\n\t"
        "mov %%bx, %%ds\n\t"
        "mov %%bx, %%es\n\t"
        "mov %%bx, %%fs\n\t"
        "mov %%bx, %%gs\n\t"
        "mov %%bx, %%ss\n\t"

        :
        : "a"(code), "b"(data)
        : "rcx", "memory");
}