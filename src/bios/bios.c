#include "bios.h"
#include "info.h"
#include "font.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static Video_mode *video_mode = &info.video_mode;

static size_t frame_index = 0;

void bios_screen_clear()
{
    for (size_t i = 0; i < video_mode->horizontal_resolution * video_mode->vertical_resolution; i++)
    {
        ((uint32_t *)video_mode->frame_buffer_base)[i] = 0;
    }

    frame_index = 0;
}

void bios_puts(const char *str, uint32_t f_color, uint32_t b_color)
{
    for (size_t i = 0; *str; i++)
    {
        if (*str == '\n')
        {
            frame_index += video_mode->horizontal_resolution * 16 - frame_index % video_mode->horizontal_resolution;
        }
        else
        {
            //输出一个字
            for (size_t v = 0; v < 16; v++)
            {
                //输出一行点阵
                for (size_t h = 0; h < 8; h++)
                {
                    if (font[(size_t)*str][v] & (1 << (7 - h)))

                        ((uint32_t *)video_mode->frame_buffer_base)[frame_index] = f_color;
                    else
                        ((uint32_t *)video_mode->frame_buffer_base)[frame_index] = b_color;

                    frame_index++;
                }

                frame_index += video_mode->horizontal_resolution - frame_index % video_mode->horizontal_resolution;
                frame_index += i * 8;
            }

            frame_index -= video_mode->horizontal_resolution * 16;
            frame_index += 8;
        }
        str++;
    }
}

void bios_message(const char *mes, int level)
{
    static const uint32_t f_color[] = {GREEN, YELLOW, RED};
    static const char *Prefix[] = {"[  Ok   ]: ", "[Warning]: ", "[ Error ]: "};

    bios_puts(Prefix[level], f_color[level], BLACK);
    bios_puts(mes, f_color[level], BLACK);
}

void bios_exit()
{
    bios_message("BIOS has exited\n", ERROR);
    while (1)
        ;
}
