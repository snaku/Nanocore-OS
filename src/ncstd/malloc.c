#include "kernel/memory/heap.h"
#include "ncstd/malloc.h"

void* malloc(size_t size)
{
    krnlMalloc(size);
}

void free(void* memory)
{
    krnlFree(memory);
}