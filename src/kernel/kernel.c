#include "kernel/interrupts/interrupts.h"
#include "kernel/interrupts/pic.h"
#include "kernel/debug/assert.h"
#include "kernel/cpu.h"
#include "drivers/vga/vga.h"
#include "terminal/terminal.h"
#include "ncstd/string.h"
#include "ncstd/memory.h"

__attribute__((noreturn))
void krnlMain()
{
    terminalInit();

    picInit();
    idtInit();

    STI();

    while (true)
    {
        terminalHandleInput();
        HLT();
    }
}