#include "kernel/cpu.h"
#include "ncstd/memory.h"

Registers g_regs;
static CpuInfo s_cpuInfo;

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

const CpuInfo* cpuGetInfo()
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    
    CPUID(0, eax, ebx, ecx, edx);
    memcpy(s_cpuInfo.vendor, &ebx, 4);
    memcpy(s_cpuInfo.vendor + 4, &edx, 4);
    memcpy(s_cpuInfo.vendor + 8, &ecx, 4);
    s_cpuInfo.vendor[12] = '\0';
    
    CPUID(0x80000002, eax, ebx, ecx, edx);
    memcpy(s_cpuInfo.name, &eax, 4);
    memcpy(s_cpuInfo.name + 4,  &ebx, 4);
    memcpy(s_cpuInfo.name + 8,  &ecx, 4);
    memcpy(s_cpuInfo.name + 12, &edx, 4);
    
    CPUID(0x80000003, eax, ebx, ecx, edx);
    memcpy(s_cpuInfo.name + 16, &eax, 4);
    memcpy(s_cpuInfo.name + 20, &ebx, 4);
    memcpy(s_cpuInfo.name + 24, &ecx, 4);
    memcpy(s_cpuInfo.name + 28, &edx, 4);
    
    CPUID(0x80000004, eax, ebx, ecx, edx);
    memcpy(s_cpuInfo.name + 32, &eax, 4);
    memcpy(s_cpuInfo.name + 36, &ebx, 4);
    memcpy(s_cpuInfo.name + 40, &ecx, 4);
    memcpy(s_cpuInfo.name + 44, &edx, 4);

    return &s_cpuInfo;
}