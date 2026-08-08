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

#define LIDT(descr) __asm__ volatile ("lidt %0" : : "m"((descr)))

extern void isr0();

static void idtSetHandler(uint8_t vec, void (*handler)())
{
    uintptr_t handlerAddr = (uintptr_t)handler;
    IdtEntry64* entry = &s_idts[vec];
    
    entry->offLow = handlerAddr & 0xffff;
    entry->selector = 0x18;
    entry->ist = 0;
    entry->typeAttr = 0x8e;
    entry->offMid = (handlerAddr >> 16) & 0xffff;
    entry->offHigh = (handlerAddr >> 32) & 0xffffffff;
    entry->zero = 0;
}

void idtInit()
{
    s_descriptor.base = (uintptr_t)s_idts;
    s_descriptor.limit = sizeof(s_idts) - 1;

    idtSetHandler(0, isr0);

    LIDT(s_descriptor);
}

void isrHandler0()
{
    vgaClear();
    vgaPuts("EXCEPTION: Divide error", VGA_COLOR_RED);

    HANG();
}