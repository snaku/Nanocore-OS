#include "kernel/vga/vga.h"
#include "kernel/cpu.h"
#include "ncstd/string.h"

void krnlMain()
{
    vgaClear();
    vgaPuts("krnlMain(): OK\n\n", VGA_COLOR_WHITE);

    char buff[32];
    strcpy(buff, "Test strcpy !!\n");
    vgaPuts(buff, VGA_COLOR_WHITE);

    for (int i = 0; i < 23; i++)
    {
        vgaPuts("Scroll test\n", VGA_COLOR_GREEN);
    }

    HANG();
}