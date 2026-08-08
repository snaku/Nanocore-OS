#pragma once

#include "ncstd/bool.h"
#include "ncstd/types.h"

#define HLT() __asm__ volatile ("hlt")

#define HANG()   \
    while (true) \
        HLT();

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

const char* cpuRegToStr(int reg);