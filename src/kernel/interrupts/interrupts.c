#include "kernel/interrupts/interrupts.h"
#include "kernel/vga/vga.h"
#include "kernel/cpu.h"

#define IDT_VECTOR_MAX 256

typedef struct __attribute__((packed)) IdtEntry64
{
    uint16_t offLow;
    uint16_t selector;
    uint8_t ist;
    uint8_t typeAttr;
    uint16_t offMid;
    uint32_t offHigh;
    uint32_t zero;
} IdtEntry64;

typedef struct __attribute__((packed)) IdtDescriptor
{
    uint16_t limit;
    uintptr_t base;
} IdtDescriptor;

static IdtEntry64 s_idts[IDT_VECTOR_MAX];
static IdtDescriptor s_descriptor;

Registers* g_regsISR; // registers values on interruption (points to RSP)

#define LIDT(descr) __asm__ volatile ("lidt %0" : : "m"((descr)))

extern void isr0();
extern void isr33();

static void idtSetVecIsr(uint8_t vec, void (*isr)())
{
    uintptr_t isrAddr = (uintptr_t)isr;
    IdtEntry64* entry = &s_idts[vec];
    
    entry->offLow = isrAddr & 0xffff;
    entry->selector = 0x18;
    entry->ist = 0;
    entry->typeAttr = 0x8e;
    entry->offMid = (isrAddr >> 16) & 0xffff;
    entry->offHigh = (isrAddr >> 32) & 0xffffffff;
    entry->zero = 0;
}

void idtInit()
{
    s_descriptor.base = (uintptr_t)s_idts;
    s_descriptor.limit = sizeof(s_idts) - 1;

    idtSetVecIsr(0x00, isr0);
    idtSetVecIsr(0x21, isr33);

    LIDT(s_descriptor);
}

// vector 0 (#DE)
void isrHandler0()
{
    vgaClear();
    vgaPuts("EXCEPTION: Divide error\n\n", VGA_COLOR_RED);

    for (int i = REG_RAX; i < REG_MAX; i++)
    {
        vgaPuts(cpuRegToStr(i), VGA_COLOR_GREEN);
        vgaPuts(": ", VGA_COLOR_GREEN);

        vgaPuthex(g_regsISR->vals[i], VGA_COLOR_WHITE);

        vgaPuts("\n", VGA_COLOR_DEFAULT);
    }

    HANG();
}

// vector 33 (IRQ1)
void isrHandler33()
{
    uint8_t scanecode = INB(0x60);

    vgaPuthex(scanecode, VGA_COLOR_WHITE);
    vgaPuts(" ", VGA_COLOR_DEFAULT);

    OUTB(0x20, 0x20); // eoi
}