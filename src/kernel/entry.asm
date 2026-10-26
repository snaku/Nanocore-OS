bits 64
global _start

extern _bssStart
extern _bssEnd

extern krnlMain

section .text.entry

_start:
    call __clearBss
    call krnlMain

__clearBss:
    cld
    mov edi, _bssStart
    mov ecx, _bssEnd
    sub ecx, edi
    xor eax, eax
    rep stosb
    ret