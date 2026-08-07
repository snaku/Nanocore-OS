#pragma once

#ifdef NCOS_DEBUG

#include "kernel/cpu.h"
#include "kernel/vga/vga.h"

#define NCOS_ASSERT(condition, msg)                       \
    do                                                    \
    {                                                     \
        if (!(condition))                                 \
        {                                                 \
            vgaClear();                                   \
            vgaPuts("ASSERTION FAILED: ", VGA_COLOR_RED); \
            vgaPuts(msg, VGA_COLOR_WHITE);                \
            vgaPuts(", ", VGA_COLOR_WHITE);               \
            vgaPuts(__FILE__, VGA_COLOR_GREEN);           \
            vgaPuts(":", VGA_COLOR_GREEN);                \
            vgaPuti(__LINE__, VGA_COLOR_GREEN);           \
            HANG();                                       \
        }                                                 \
    } while (0)

#else
#define NCOS_ASSERT(condition, msg)
#endif

