bits 16
org 0x8000

start_2:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov si, startMsg
    call print2

    ; read kernel
    mov ah, 0x42
    mov dl, 0x80

    mov si, dapPacket
    int 0x13
    jc krnlReadErr

    xor ax, ax
    mov es, ax

    mov si, krnlReadOkMsg
    call print2

    cli

    lgdt [gdtDescriptor] ; load gdt

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:protectedMode

krnlReadErr:
    mov si, krnlReadErrMsg
    call print2
    jmp hang2

; protected mode

bits 32
protectedMode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x9f000

    mov dword [0xb8000], 0x1f4b1f4f ; "OK"

    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, (26 * 512) / 4
    cld
    rep movsd

    ; pae in cr4
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, pml4
    mov cr3, eax

    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; paging activate
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp 0x18:longMode

; long mode

bits 64
longMode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, 0x9f000

    mov dword [0xb8004], 0x2f342f36 ; "64"

    mov rax, 0x100000
    call rax ; _start

hang2:
    hlt
    jmp hang2

print2:
    lodsb
    cmp al, 0
    jne .notDone
    ret
.notDone:
    mov ah, 0x0e
    int 0x10
    jmp print2

; gdt

gdtStart:

gdtNull:
    dq 0

; 32 bits
gdtCode32:
    dw 0xffff
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

gdtData:
    dw 0xffff
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

; 64 bits
gdtCode64:
    dw 0xffff
    dw 0
    db 0
    db 10011010b
    db 10101111b
    db 0

gdtEnd:

gdtDescriptor:
    dw gdtEnd - gdtStart - 1
    dd gdtStart

align 4096
pml4:
    dq pdpt + 0x03

align 4096
pdpt:
    dq pd + 0x03

align 4096
pd:
    dq 0x00000083

align 4
dapPacket:
    db 0x10
    db 0x00
    dw 0x1a
    dw 0x0000
    dw 0x1000
    dq 0x1a

startMsg db "BOOT 2 START", 13, 10, 0
krnlReadErrMsg db "ERROR READ KERNEL", 13, 10, 0
krnlReadOkMsg db "KERNEL READ OK", 13, 10, 0

times 12800 - ($ - $$) db 0