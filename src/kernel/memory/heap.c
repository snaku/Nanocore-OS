#include "kernel/memory/heap.h"
#include "terminal/terminal.h"
#include "ncstd/memory.h"
#include "ncstd/bool.h"

extern char _heapStart;

#define HEAP_START ((uintptr_t)&_heapStart)
#define HEAP_SIZE   0x5000
#define HEAP_END   (HEAP_START + HEAP_SIZE)

typedef struct MemBlock MemBlock;
struct MemBlock
{
    size_t size;
    MemBlock* next;
    MemBlock* prev;
    ncbool isUsed;
};

static MemBlock* s_blockHead = NULL;

void* krnlMalloc(size_t size)
{
    if (size + sizeof(MemBlock) > HEAP_SIZE)
    {
        return NULL;
    }

    size_t sizeMod = size % sizeof(void*);
    if (sizeMod != 0)
    {
        size += sizeof(void*) - sizeMod; // add padding
    }

    MemBlock* block = NULL;
    if (s_blockHead == NULL) // first malloc
    {
        s_blockHead = (MemBlock*)HEAP_START;

        block = s_blockHead;

        block->prev = NULL;
        block->next = NULL;
        block->size = size;
    }
    else
    {
        MemBlock* currBlock = s_blockHead;
        MemBlock* prevBlock = NULL;
        while (currBlock != NULL)
        {
            // first fit
            if (!currBlock->isUsed &&
                currBlock->size >= size)
            {
                break;
            }

            prevBlock = currBlock;
            currBlock = currBlock->next;
        }

        if (currBlock == NULL)
        {
            block = (MemBlock*)((uintptr_t)prevBlock + (prevBlock->size + sizeof(MemBlock))); // create new block

            prevBlock->next = block;
            block->prev = prevBlock;
            block->next = NULL;
        }
        else
        {
            block = currBlock;
        }
    }

    block->isUsed = true;

    return (void*)((uintptr_t)block + sizeof(MemBlock));
}

void krnlFree(void* memory)
{
    if (memory == NULL)
    {
        return;
    }

    MemBlock* block = (MemBlock*)((uintptr_t)memory - sizeof(MemBlock));
    if ((uintptr_t)block < HEAP_START ||
        (uintptr_t)block >= HEAP_END)
    {
        return;
    }

    block->isUsed = false;
}