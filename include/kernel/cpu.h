#pragma once

#include "ncstd/bool.h"
#include "ncstd/types.h"

#define HLT() __asm__ volatile ("hlt")

#define HANG()   \
    while (true) \
        HLT();

#define OUTB(port, val)   __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port))
#define INB(port) ({uint8_t val; __asm__ volatile ("inb %w1, %b0" : "=a"(val) : "Nd"((port))); val;})

#define STI() __asm__ volatile ("sti")
#define CLI() __asm__ volatile ("cli")

#define CPUID(leaf, eax, ebx, ecx, edx) \
    asm volatile (                      \
        "cpuid"                         \
        : "=a"(eax),                    \
          "=b"(ebx),                    \
          "=c"(ecx),                    \
          "=d"(edx)                     \
        : "a"(leaf),                    \
          "c"(0))

enum Register
{
    REG_RAX,
    REG_RBX,
    REG_RCX,
    REG_RDX,
    REG_RSI,
    REG_RDI,
    REG_RBP,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    
    REG_MAX
};

typedef struct Registers
{
    uint64_t vals[REG_MAX];
} Registers;

typedef struct CpuInfo
{
    char vendor[13];
    char name[49];
} CpuInfo;

extern const Registers* cpuGetRegs(); // see kernel/cpu.asm

const char* cpuRegToStr(int reg);
const CpuInfo* cpuGetInfo();