bits 16
org 0x7c00

start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov [boot_drive], dl  ; save boot drive no

    sti

    mov si, ncosMsg
    call print

    mov ah, 0x02 ; read sector
    mov al, 0x19 ; 25 sectors for boot_2
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, [boot_drive]

    mov bx, 0x0800
    mov es, bx
    xor bx, bx

    int 0x13
    jc readSectorErr

    ; no error

    mov si, successMsg
    call print

    jmp 0x0000:0x8000 ; to stage2_start

readSectorErr:
    mov si, errorMsg
    call print

hang:
    hlt
    jmp hang

print:
    lodsb
    cmp al, 0
    jne .not_done
    ret

.not_done:
    mov ah, 0x0e
    mov bh, 0
    int 0x10
    jmp print

ncosMsg    db "*!!!!* ncOS *!!!!*", 13, 10, 0
errorMsg   db "ERROR", 13, 10, 0
successMsg db "OK", 13, 10, 0
boot_drive db 0


times 510 - ($ - $$) db 0
dw 0xaa55 ; magic