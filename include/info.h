#pragma once

#include <stdint.h>

typedef struct
{
    uint32_t type;
    uint32_t pad;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t number_of_pages;
    uint64_t attribute;
} Descriptor;

typedef struct
{
    Descriptor *descriptor;
    size_t size;
} Memory_map;

typedef struct
{
    void *frame_buffer_base;
    size_t frame_buffer_size;
    size_t horizontal_resolution;
    size_t vertical_resolution;
    size_t pixels_per_scanLine;
} Video_mode;

typedef struct
{
    Memory_map memory_map;
    Video_mode video_mode;
} Info;

extern Info info;