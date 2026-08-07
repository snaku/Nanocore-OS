#include "ncstd/bool.h"
#include "kernel/vga/vga.h"

void krnlMain()
{
    vgaClear();
    vgaPuts("krnlMain(): OK\n\n", VGA_COLOR_WHITE);
    vgaPuts("Test !", VGA_COLOR_GREEN);

    while (true)
    {
        __asm__ volatile ("hlt");
    }
}