#include "kernel/vga/vga.h"
#include "kernel/interrupts/interrupts.h"
#include "kernel/interrupts/pic.h"
#include "kernel/debug/assert.h"
#include "kernel/cpu.h"
#include "ncstd/string.h"
#include "ncstd/memory.h"

void krnlMain()
{
    vgaClear();
    vgaPuts("krnlMain(): OK\n\n", VGA_COLOR_WHITE);

    STI();

    picInit();
    idtInit();

    HANG();
}