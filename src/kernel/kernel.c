#include "kernel/vga/vga.h"
#include "kernel/interrupts/interrupts.h"
#include "kernel/debug/assert.h"
#include "kernel/cpu.h"
#include "ncstd/string.h"
#include "ncstd/memory.h"

void krnlMain()
{
    vgaClear();
    vgaPuts("krnlMain(): OK\n\n", VGA_COLOR_WHITE);

    idtInit();

    int a = 0;
    int b = 1;
    int c = b / a; // #DE test

    for (int i = 0; i < 23; i++)
    {
        vgaPuts("Scroll test\n", VGA_COLOR_GREEN);
    }

    int* p = NULL;
    NCOS_ASSERT(p != NULL, "p == NULL (TEST)");

    HANG();
}