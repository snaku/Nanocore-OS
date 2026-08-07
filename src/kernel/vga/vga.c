#include "kernel/vga/vga.h"
#include "ncstd/string.h"

#define VGA_COL_MAX  80
#define VGA_LINE_MAX 25
#define VGA_SET(col, c) (sv_vgaAddr[s_vga.cursor++] = (((unsigned short)(col)) << 8) | (unsigned char)(c))

typedef struct Vga
{
    int cursor;
    int currCol;
} Vga;

static volatile unsigned short* sv_vgaAddr = (volatile unsigned short*)0xb8000;

static Vga s_vga;

void vgaPuts(const char* str, int color)
{
    if (color >= VGA_COLOR_MAX)
    {
        return;
    }

    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        char c = str[i];

        if (c == '\n')
        {
            s_vga.cursor += (VGA_COL_MAX - s_vga.currCol);
            s_vga.currCol = 0;

            continue;
        }

        if (++s_vga.currCol >= VGA_COL_MAX)
        {
            s_vga.currCol = 0;
        }

        VGA_SET(color, c);
    }
}

void vgaClear()
{
    for (int i = 0; i < VGA_COL_MAX * VGA_LINE_MAX; i++)
    {
        VGA_SET(VGA_COLOR_DEFAULT, ' ');
    }

    s_vga.cursor = 0;
    s_vga.currCol = 0;
}