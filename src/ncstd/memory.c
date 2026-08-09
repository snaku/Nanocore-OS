#include "ncstd/memory.h"

void* memset(void* dst, int value, size_t size)
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
    uint8_t val8 = (uint8_t)value;

    if ((uintptr_t)dst8 % sizeof(uint64_t) == 0)
    {
        size_t size64 = size / sizeof(uint64_t);
        uint64_t val64 = ((uint64_t)val8 << 56) |
                         ((uint64_t)val8 << 48) |
                         ((uint64_t)val8 << 40) |
                         ((uint64_t)val8 << 32) |
                         ((uint64_t)val8 << 24) |
                         ((uint64_t)val8 << 16) |
                         ((uint64_t)val8 << 8)  |
                         (uint64_t)val8;

        for (size_t i = 0; i < size64; i++)
        {
            ((uint64_t*)dst8)[i] = val64;
        }

        dst8 += size64 * sizeof(uint64_t);
        size -= size64 * sizeof(uint64_t);
    }

    while (size-- != 0)
    {
        *dst8++ = val8;
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