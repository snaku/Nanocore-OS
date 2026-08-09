#pragma once

#include "ncstd/types.h"

#define NULL ((void*)0)

void* memset(void* dst, int value, size_t size);
void* memcpy(void* dst, const void* src, size_t size);