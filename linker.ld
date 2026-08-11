ENTRY(_start)

KERNEL_VMA = 0x100000;
KERNEL_LMA = 0x100000;

SECTIONS
{
    . = KERNEL_VMA;

    .text ALIGN(4K) : AT(KERNEL_LMA)
    {
        *(.text.entry)
        *(.text*)
    }

    .rodata ALIGN(4K) : AT(KERNEL_LMA + (ADDR(.rodata) - KERNEL_VMA))
    {
        *(.rodata*)
    }

    .data ALIGN(4K) : AT(KERNEL_LMA + (ADDR(.data) - KERNEL_VMA))
    {
        *(.data*)
    }

    .bss ALIGN(4K) : AT(KERNEL_LMA + (ADDR(.bss) - KERNEL_VMA))
    {
        *(COMMON)
        *(.bss*)
    }

    .eh_frame ALIGN(4K) : AT(KERNEL_LMA + (ADDR(.eh_frame) - KERNEL_VMA))
    {
        *(.eh_frame*)
    }

    _heapStart = .;
}
