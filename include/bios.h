#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define BLACK 0x00000000
#define RED 0X00ff0000
#define GREEN 0x0000ff00
#define BLUE 0x000000ff
#define YELLOW 0x00ffff00
#define WHITE 0x00ffffff

#define OK 0x0
#define WARNING 0X1
#define ERROR 0X2

extern void bios_screen_clear();

extern void bios_puts(const char *str, uint32_t f_color, uint32_t b_color);

extern void bios_message(const char *mes, int level);

extern void bios_exit();