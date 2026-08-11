#pragma once

#include "ncstd/types.h"

void* krnlMalloc(size_t size);
void krnlFree(void* memory);