#include "ncstd/memory.h"

void* memset(void* dst, uint64_t value, size_t size)
{
    if (dst == NULL)
    {
        return NULL;
    }

    if (size == 0)
    {
        return dst;
    }

    uint8_t* dst8 = (uint8_t*)dst;

    if ((uintptr_t)dst8 % sizeof(uint64_t) == 0)
    {
        size_t size64 = size / sizeof(uint64_t);

        for (size_t i = 0; i < size64; i++)
        {
            ((uint64_t*)dst8)[i] = value;
        }

        dst8 += size64 * sizeof(uint64_t);
        size -= size64 * sizeof(uint64_t);
    }

    while (size-- != 0)
    {
        *dst8++ = (uint8_t)value;
    }

    return dst;
}

void* memcpy(void* dst, const void* src, size_t size)
{
    if (dst == NULL ||
        src == NULL)
    {
        return NULL;
    }

    if (size == 0)
    {
        return dst;
    }

    uint8_t* dst8 = (uint8_t*)dst;
    const uint8_t* src8 = (const uint8_t*)src;

    if ((uintptr_t)dst8 % sizeof(uint64_t) == 0 &&
        (uintptr_t)src8 % sizeof(uint64_t) == 0)
    {
        size_t size64 = size / sizeof(uint64_t);

        for (size_t i = 0; i < size64; i++)
        {
            ((uint64_t*)dst8)[i] = ((uint64_t*)src8)[i];
        }

        dst8 += size64 * sizeof(uint64_t);
        src8 += size64 * sizeof(uint64_t);
        size -= size64 * sizeof(uint64_t);
    }

    while (size-- != 0)
    {
        *dst8++ = *src8++;
    }

    return dst;
}