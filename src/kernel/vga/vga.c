#include "kernel/vga/vga.h"
#include "ncstd/string.h"

#define VGA_COL_MAX  80
#define VGA_LINE_MAX 25

typedef struct Vga
{
    int cursor;
    int currCol;
    int currLine;
} Vga;

static volatile unsigned short* sv_vgaAddr = (volatile unsigned short*)0xb8000;

static Vga s_vga;

static void vgaScroll()
{
    for (int i = 1; i < VGA_LINE_MAX; i++)
    {
        for (int j = 0; j < VGA_COL_MAX; j++)
        {
            sv_vgaAddr[(i - 1) * VGA_COL_MAX + j] = sv_vgaAddr[i * VGA_COL_MAX + j];
        }
    }

    for (int i = 0; i < VGA_COL_MAX; i++)
    {
        sv_vgaAddr[(VGA_LINE_MAX - 1) * VGA_COL_MAX + i] = ((unsigned short)VGA_COLOR_DEFAULT << 8) | ' ';
    }

    s_vga.cursor = (VGA_LINE_MAX - 1) * VGA_COL_MAX;
    s_vga.currCol = 0;
    s_vga.currLine = VGA_LINE_MAX - 1;
}

void vgaPutchar(char c, int color)
{
    sv_vgaAddr[s_vga.cursor++] = (color << 8) | c;

    if (++s_vga.currCol >= VGA_COL_MAX)
    {
        s_vga.currCol = 0;
        s_vga.currLine++;
    }

    if (s_vga.currLine >= VGA_LINE_MAX)
    {
        vgaScroll();
    }
}

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

            if (++s_vga.currLine >= VGA_LINE_MAX)
            {
                vgaScroll();
            }

            continue;
        }

        vgaPutchar(c, color);
    }
}

void vgaClear()
{
    for (int i = 0; i < VGA_COL_MAX * VGA_LINE_MAX; i++)
    {
        vgaPutchar(' ', VGA_COLOR_DEFAULT);
    }

    s_vga.cursor = 0;
    s_vga.currCol = 0;
    s_vga.currLine = 0;
}