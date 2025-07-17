/*
 * A custom implementation of malloc and free with realloc and
 * calloc features based on FreeRTOS heap4.c.
 */
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#define HEAP_SIZE 10000

#if (configSUPPORT_DYNAMIC_ALLOCATION == 0)
#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
#define HEAP_STRUCT_SIZE (size_t)((sizeof(BlockLink_t) + ((size_t)(portBYTE_ALIGNMENT - 1))) & ~((size_t)portBYTE_ALIGNMENT_MASK))

/* Block sizes must not get too small. */
#define HEAP_MIN_BLOCK_SIZE ((size_t)(HEAP_STRUCT_SIZE << 1))

/* Assumes 8bit bytes! */
#define HEAP_BITS_PER_BYTE ((size_t)8)

/* Allocate the memory for the heap. */
static uint8_t Uros_heap[HEAP_SIZE];

/* Define the linked list structure. This is used to link free blocks in order
of their memory address. */
typedef struct BlockLink {
    struct BlockLink *pxNextFreeBlock; /*<< The next free block in the list. */
    size_t xBlockSize;                 /*<< The size of the free block. */
} BlockLink_t;

/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void InsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert);

/*
 * Called automatically to setup the required heap structures the first time
 * pvPortMallocMicroROS() is called.
 */
static void HeapInit(void);

/*-----------------------------------------------------------*/

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t Uros_start = { 0 };
static BlockLink_t *Uros_end = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t Uros_heap_free_size = 0U;
static size_t Uros_heap_min_free_size = 0U;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

/*-----------------------------------------------------------*/

void *Uros_MallocImpl(size_t xWantedSize) {
    BlockLink_t *pxBlock = NULL;
    BlockLink_t *pxPreviousBlock = NULL;
    BlockLink_t *pxNewBlockLink = NULL;
    void *pvReturn = NULL;

    vTaskSuspendAll();

    /* If this is the first call to malloc then the heap will require
    initialisation to setup the list of free blocks. */
    if (Uros_end == NULL) {
        HeapInit();
    } else {
        mtCOVERAGE_TEST_MARKER();
    }

    /* Check the requested block size is not so large that the top bit is
    set.  The top bit of the block size member of the BlockLink_t structure
    is used to determine who owns the block - the application or the
    kernel, so it must be free. */
    if ((xWantedSize & xBlockAllocatedBit) != 0) {
        mtCOVERAGE_TEST_MARKER();
        goto Finally;
    }
    /* The wanted size is increased so it can contain a BlockLink_t
    structure in addition to the requested amount of bytes. */
    if (xWantedSize > 0) {
        xWantedSize += HEAP_STRUCT_SIZE;

        /* Ensure that blocks are always aligned to the required number
        of bytes. */
        if ((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00) {
            /* Byte alignment required. */
            xWantedSize += portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK);
            configASSERT((xWantedSize & portBYTE_ALIGNMENT_MASK) == 0);
        } else {
            mtCOVERAGE_TEST_MARKER();
        }
    } else {
        mtCOVERAGE_TEST_MARKER();
    }

    if (xWantedSize <= 0 || xWantedSize > Uros_heap_free_size) {
        mtCOVERAGE_TEST_MARKER();
        goto Finally;
    }
    /* Traverse the list from the start (lowest address) block until
    one of adequate size is found. */
    pxPreviousBlock = &Uros_start;
    pxBlock = Uros_start.pxNextFreeBlock;
    while (pxBlock->xBlockSize < xWantedSize && pxBlock->pxNextFreeBlock != NULL) {
        pxPreviousBlock = pxBlock;
        pxBlock = pxBlock->pxNextFreeBlock;
    }

    /* If the end marker was reached then a block of adequate size
    was not found. */
    if (pxBlock == Uros_end) {
        mtCOVERAGE_TEST_MARKER();
        goto Finally;
    }
    /* Return the memory space pointed to - jumping over the
    BlockLink_t structure at its start. */
    pvReturn = (uint8_t *)pxPreviousBlock->pxNextFreeBlock + HEAP_STRUCT_SIZE;

    /* This block is being returned for use so must be taken out
    of the list of free blocks. */
    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

    /* If the block is larger than required it can be split into
    two. */
    if (pxBlock->xBlockSize - xWantedSize > HEAP_MIN_BLOCK_SIZE) {
        /* This block is to be split into two.  Create a new
        block following the number of bytes requested. The void
        cast is used to prevent byte alignment warnings from the
        compiler. */
        pxNewBlockLink = (void *)(((uint8_t *)pxBlock) + xWantedSize);
        configASSERT((((size_t)pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0);

        /* Calculate the sizes of two blocks split from the
        single block. */
        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
        pxBlock->xBlockSize = xWantedSize;

        /* Insert the new block into the list of free blocks. */
        InsertBlockIntoFreeList(pxNewBlockLink);
    } else {
        mtCOVERAGE_TEST_MARKER();
    }

    Uros_heap_free_size -= pxBlock->xBlockSize;

    if (Uros_heap_free_size < Uros_heap_min_free_size) {
        Uros_heap_min_free_size = Uros_heap_free_size;
    } else {
        mtCOVERAGE_TEST_MARKER();
    }

    /* The block is being returned - it is allocated and owned
    by the application and has no "next" block. */
    pxBlock->xBlockSize |= xBlockAllocatedBit;
    pxBlock->pxNextFreeBlock = NULL;

Finally:
    traceMALLOC(pvReturn, xWantedSize);

    xTaskResumeAll();

#if (configUSE_MALLOC_FAILED_HOOK == 1)
    {
        if (pvReturn == NULL) {
            extern void vApplicationMallocFailedHook(void);
            vApplicationMallocFailedHook();
        } else {
            mtCOVERAGE_TEST_MARKER();
        }
    }
#endif

    configASSERT((((size_t)pvReturn) & (size_t)portBYTE_ALIGNMENT_MASK) == 0);
    return pvReturn;
}
/*-----------------------------------------------------------*/

void Uros_FreeImpl(void *pv) {
    uint8_t *puc = pv;
    BlockLink_t *pxLink = NULL;

    if (pv == NULL) {
        return;
    }
    /* The memory being freed will have an BlockLink_t structure immediately
    before it. */
    puc -= HEAP_STRUCT_SIZE;

    /* This casting is to keep the compiler from issuing warnings. */
    pxLink = (void *)puc;

    /* Check the block is actually allocated. */
    configASSERT((pxLink->xBlockSize & xBlockAllocatedBit) != 0);
    configASSERT(pxLink->pxNextFreeBlock == NULL);

    if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0) {
        if (pxLink->pxNextFreeBlock == NULL) {
            /* The block is being returned to the heap - it is no longer
            allocated. */
            pxLink->xBlockSize &= ~xBlockAllocatedBit;

            vTaskSuspendAll();
            {
                /* Add this block to the list of free blocks. */
                Uros_heap_free_size += pxLink->xBlockSize;
                traceFREE(pv, pxLink->xBlockSize);
                InsertBlockIntoFreeList(((BlockLink_t *)pxLink));
            }
            xTaskResumeAll();
        } else {
            mtCOVERAGE_TEST_MARKER();
        }
    } else {
        mtCOVERAGE_TEST_MARKER();
    }
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
size_t Uros_GetBlockSize(void *pv) {
    uint8_t *puc = pv;
    BlockLink_t *pxLink = NULL;

    puc -= HEAP_STRUCT_SIZE;
    pxLink = (void *)puc;

    return pxLink->xBlockSize & ~xBlockAllocatedBit;
}
/*-----------------------------------------------------------*/

void *Uros_ReallocImpl(void *pv, size_t xWantedSize) {
    vTaskSuspendAll();

    void *newmem = Uros_MallocImpl(xWantedSize);
    if (newmem != NULL && pv != NULL) {
        size_t count = Uros_GetBlockSize(pv) - HEAP_STRUCT_SIZE;
        if (xWantedSize < count) {
            count = xWantedSize;
        }
        memcpy(newmem, pv, count);

        Uros_FreeImpl(pv);
    }

    xTaskResumeAll();

    return newmem;
}
/*-----------------------------------------------------------*/

void *Uros_CallocImpl(size_t num, size_t xWantedSize) {
    vTaskSuspendAll();
    size_t count = xWantedSize * num;

    void *mem = Uros_MallocImpl(count);
    char *in_dest = (char *)mem;

    while (count--) {
        *in_dest++ = 0;
    }

    xTaskResumeAll();
    return mem;
}
/*-----------------------------------------------------------*/

size_t Uros_GetFreeHeapSize(void) {
    return Uros_heap_free_size;
}
/*-----------------------------------------------------------*/

size_t Uros_GetMinFreeHeapSize(void) {
    return Uros_heap_min_free_size;
}
/*-----------------------------------------------------------*/

static void HeapInit(void) {
    BlockLink_t *pxFirstFreeBlock = NULL;
    uint8_t *pucAlignedHeap = NULL;
    size_t uxAddress = 0;
    size_t xTotalHeapSize = HEAP_SIZE;

    /* Ensure the heap starts on a correctly aligned boundary. */
    uxAddress = (size_t)Uros_heap;

    if ((uxAddress & portBYTE_ALIGNMENT_MASK) != 0) {
        uxAddress += (portBYTE_ALIGNMENT - 1);
        uxAddress &= ~((size_t)portBYTE_ALIGNMENT_MASK);
        xTotalHeapSize -= uxAddress - (size_t)Uros_heap;
    }

    pucAlignedHeap = (uint8_t *)uxAddress;

    /* xStart is used to hold a pointer to the first item in the list of free
    blocks.  The void cast is used to prevent compiler warnings. */
    Uros_start.pxNextFreeBlock = (void *)pucAlignedHeap;
    Uros_start.xBlockSize = (size_t)0;

    /* pxEnd is used to mark the end of the list of free blocks and is inserted
    at the end of the heap space. */
    uxAddress = ((size_t)pucAlignedHeap) + xTotalHeapSize;
    uxAddress -= HEAP_STRUCT_SIZE;
    uxAddress &= ~((size_t)portBYTE_ALIGNMENT_MASK);
    Uros_end = (void *)uxAddress;
    Uros_end->xBlockSize = 0;
    Uros_end->pxNextFreeBlock = NULL;

    /* To start with there is a single free block that is sized to take up the
    entire heap space, minus the space taken by pxEnd. */
    pxFirstFreeBlock = (void *)pucAlignedHeap;
    pxFirstFreeBlock->xBlockSize = uxAddress - (size_t)pxFirstFreeBlock;
    pxFirstFreeBlock->pxNextFreeBlock = Uros_end;

    /* Only one block exists - and it covers the entire usable heap space. */
    Uros_heap_min_free_size = pxFirstFreeBlock->xBlockSize;
    Uros_heap_free_size = pxFirstFreeBlock->xBlockSize;

    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ((size_t)1) << ((sizeof(size_t) * HEAP_BITS_PER_BYTE) - 1);
}
/*-----------------------------------------------------------*/

static void InsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert) {
    BlockLink_t *pxIterator = NULL;
    uint8_t *puc = NULL;

    /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
    for (
        pxIterator = &Uros_start;
        pxIterator->pxNextFreeBlock < pxBlockToInsert;
        pxIterator = pxIterator->pxNextFreeBlock
    ) {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
    puc = (uint8_t *)pxIterator;
    if ((puc + pxIterator->xBlockSize) == (uint8_t *)pxBlockToInsert) {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    } else {
        mtCOVERAGE_TEST_MARKER();
    }

    /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
    puc = (uint8_t *)pxBlockToInsert;
    if ((puc + pxBlockToInsert->xBlockSize) == (uint8_t *)pxIterator->pxNextFreeBlock) {
        if (pxIterator->pxNextFreeBlock != Uros_end) {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        } else {
            pxBlockToInsert->pxNextFreeBlock = Uros_end;
        }
    } else {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
    if (pxIterator != pxBlockToInsert) {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    } else {
        mtCOVERAGE_TEST_MARKER();
    }
}
