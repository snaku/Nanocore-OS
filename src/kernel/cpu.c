#include "kernel/cpu.h"

Registers g_regs;

const char* cpuRegToStr(int reg)
{
    switch (reg)
    {
        case REG_RAX: return "RAX";
        case REG_RBX: return "RBX";
        case REG_RCX: return "RCX";
        case REG_RDX: return "RDX";
        case REG_RSI: return "RSI";
        case REG_RDI: return "RDI";
        case REG_RBP: return "RBP";

        case REG_R8:  return "R8";
        case REG_R9:  return "R9";
        case REG_R10: return "R10";
        case REG_R11: return "R11";
        case REG_R12: return "R12";
        case REG_R13: return "R13";
        case REG_R14: return "R14";
        case REG_R15: return "R15";
    }

    return "UNK REG";
}