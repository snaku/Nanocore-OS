global cpuGetRegs

extern g_regs ; see kernel/cpu.c

cpuGetRegs:
    mov [rel g_regs + 0],   rax
    mov [rel g_regs + 8],   rbx
    mov [rel g_regs + 16],  rcx
    mov [rel g_regs + 24],  rdx
    mov [rel g_regs + 32],  rsi
    mov [rel g_regs + 40],  rdi
    mov [rel g_regs + 48],  rbp
    mov [rel g_regs + 56],  r8
    mov [rel g_regs + 64],  r9
    mov [rel g_regs + 72],  r10
    mov [rel g_regs + 80],  r11
    mov [rel g_regs + 88],  r12
    mov [rel g_regs + 96],  r13
    mov [rel g_regs + 104], r14
    mov [rel g_regs + 112], r15

    lea rax, [rel g_regs]
    ret