#pragma once

#include "ncstd/bool.h"

#define HLT() __asm__ volatile ("hlt")

#define HANG()   \
    while (true) \
        HLT();
    