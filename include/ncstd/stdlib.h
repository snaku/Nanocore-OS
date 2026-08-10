#pragma once

#include "ncstd/types.h"

char* itoa(int64_t value, char* dst, int base);
uint64_t strtoull(const char* str, char** endPtr, int base);