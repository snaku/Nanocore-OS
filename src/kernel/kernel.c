#include "ncstd/bool.h"
#include "kernel/vga/vga.h"

void krnlMain()
{
    vgaClear();
    vgaPuts("krnlMain(): OK\n\n", VGA_COLOR_WHITE);
    vgaPuts("TEST !\n", VGA_COLOR_WHITE);
    vgaPuts("TEST 2 !\n", VGA_COLOR_WHITE);

    for (int i = 0; i < 23; i++)
    {
        vgaPuts("Scroll test\n", VGA_COLOR_GREEN);
    }

    while (true)
    {
        __asm__ volatile ("hlt");
    }
}