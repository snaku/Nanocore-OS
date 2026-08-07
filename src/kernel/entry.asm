bits 64
global _start

extern krnlMain

section .text.entry

_start:
    call krnlMain