#include <stdint.h>

extern void init_gdt();

extern void set_gdt(uint16_t code, uint16_t data);