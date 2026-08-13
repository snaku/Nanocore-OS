#include "kernel/memory/heap.h"
#include "ncstd/malloc.h"

void* malloc(size_t size)
{
    return krnlMalloc(size);
}

void free(void* memory)
{
    krnlFree(memory);
}