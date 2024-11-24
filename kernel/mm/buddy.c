/**
 * Copyright (C) 2024 Romain CADILHAC
 *
 * This file is part of Kiwi
 *
 * Kiwi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kiwi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kiwi. If not, see <http://www.gnu.org/licenses/>.
 */
#include <mm/page.h>
#include <mm/buddy.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <arch/paging.h>

/**
 * @brief The buddy allocator buckets free lists. Each bucket represents a
 * block size that the buddy allocator will manage.
 */
static struct list_head buddy_buckets[BUDDY_BUCKET_COUNT] = { };

/**
 * @brief The buddy allocator initialization flag. This flag is used to modify
 * the behaviour of the `buddy_free()` function when the buddy allocator is not
 * initialized. When the buddy allocator is not initialized, the `buddy_free()`
 * function will allow freeing memory with the `PG_FREE` flag set, which is a
 * critical error after the buddy allocator initialization, but is necessary
 * before the buddy allocator is initialized.
 */
static bool buddy_initialized = false;

/**
 * @brief Create a buddy block at the specified base address and initialize it.
 * It simply creates a new `buddy_block` structure at the given base address 
 * without adding it to any free list or modifying the page information for the
 * block.
 * 
 * @param base The base address of the block.
 * @return struct buddy_block* The created buddy block at the given
 * base address.
 */
static struct buddy_block *create_buddy_block_at(vaddr base) {
    struct buddy_block *block = (struct buddy_block *) base;
    list_init(&block->list);
    return block;
}

/**
 * @brief Get the address of the buddy block for the given block. The buddy 
 * block is the block that can be coalesced with the given block to form a
 * larger block.
 * 
 * @param base The base address of the block.
 * @param order The page order.
 * @return u32 The address of the buddy block.
 */
static u32 buddy_address(vaddr base, u32 order) {
    return base ^ (1 << (order + 12));
}

/**
 * @brief Get the number of pages in the given order block.
 * 
 * @param order The order of the block. 
 * @return u32 The number of pages in the block.
 */
static u32 buddy_order_to_pfn(u32 order) {
    return 1 << order;
}

/**
 * @brief Convert a virtual address handled by the buddy allocator to a physical
 * address.
 * 
 * @param addr The virtual address. It must be a valid address managed by the
 * buddy allocator.
 * @return paddr The physical address.
 */
static paddr buddy_vaddr_to_paddr(vaddr addr) {
    return addr - KERNEL_VBASE;
}

/**
 * @brief Verify if the block with the given base address can be coalesced with
 * the its buddy block. The block can be coalesced if with its buddy block if:
 *  - The buddy block exists in memory (i.e. it is not outside the range of the
 *   physical memory).
 *  - The block and its buddy block are both free.
 *  - The block and its buddy block have the same order.
 *  - Coalescing the block with its buddy block will not exceed the maximum
 *    order of the buddy allocator.
 * 
 * @note This function assume that the `vbase` parameter points to the base
 * of a block managed by the buddy allocator (i.e the base page has a non-zero
 * order).
 * 
 * @param base The base address of the block.
 * @return true If the block can be coalesced with its buddy block.
 * @return false If the block cannot be coalesced with its buddy block.
 */
static bool buddy_can_coalesce(vaddr vbase) {
    const paddr base = buddy_vaddr_to_paddr(vbase);
    assert(page_info(base) != NULL);
    struct page *pg = page_info(base);
    struct page *buddy = page_info(buddy_address(base, pg->order));
    return (buddy != NULL) &&
           (pg->flags & PG_FREE) &&
           (pg->flags & PG_BUDDY) && 
           (buddy->flags & PG_FREE) &&
           (buddy->flags & PG_BUDDY) &&
           (pg->order == buddy->order) &&
           (pg->order < BUDDY_MAX_ORDER);
}

/**
 * @brief Print some debug information about the buddy allocator. This function
 * is useful to check the state of the buddy allocator and to debug issues with
 * the allocator.
 */
void buddy_debug(void) {
    for (int i = 0; i < BUDDY_BUCKET_COUNT; i++) {
        struct list_head *bucket = &buddy_buckets[i];
        debug("Bucket #%u (%u KiB block):", i, 4 << i);
        if (list_empty(bucket)) {
            continue;
        }

        list_foreach(bucket, entry) {
            struct buddy_block * block = list_entry(
                entry, struct buddy_block, list);
            debug("  - Block %p-%p", block, (vaddr) block + (1 << (i + 12)));
        }
    }
}

/**
 * @brief Setup the buddy allocator. It simply initializes the free lists for
 * each bucket. Each bucket is still empty at this point, and memory must be
 * added to the buddy allocator using the `buddy_free()` function to make it
 * available for allocation.
 */
_init
void buddy_setup(void) {
    // Initialize the free lists for each bucket to an empty list.
    for (int i = 0; i < BUDDY_BUCKET_COUNT; i++) {
        list_init(&buddy_buckets[i]);
    }

    // Use the page array to find free pages and add them to the buddy
    // allocator (by default, the buddy allocator does not contain any
    // free pages: this is because the buddy allocator is not aware of the
    // available/used/reserved pages in the system)
    for (int i = 0; i < BUDDY_MAX_PAGES; i++) {
        struct page *pg = page_pfn_info(i);
        if (pg == NULL) {
            break;
        } else if (pg->flags & PG_FREE) {
            pg->flags |= PG_BUDDY;
            buddy_free((void *) KERNEL_VBASE + page_pnf_to_offset(i), 0);
        }
    }
}

/**
 * @brief Free a block of memory allocated by the buddy allocator. The block
 * must have been allocated by the buddy allocator and must not have been
 * freed before. If the block has already been freed, this function will panic.
 * 
 * @note Passing a NULL pointer to this function is safe and has no effect: the
 * function will simply return without doing anything.
 * 
 * @param ptr The base address of the block to free.
 */
void buddy_free(void *ptr, u32 order)
{
    vaddr base = (vaddr) ptr;
    if (ptr == NULL) {
        return;
    }

    // Update the page information for the block by removing the PG_KERNEL
    // flag if it is set and adding the PG_FREE flag. It also set the `order`
    // field to 0 to indicate that this not a valid block anymore.
    paddr pbase = buddy_vaddr_to_paddr(base);
    for (u32 i = 0; i < buddy_order_to_pfn(order); i++) {
        struct page *page = page_info(pbase + (i << PAGE_SHIFT));
        assert(!(page->flags & PG_FREE) || !buddy_initialized);
        assert(!(page->flags & PG_RESERVED));
        assert(!(page->flags & PG_POISONED));
        page->flags &= ~PG_KERNEL;
        page->flags |= PG_FREE;
        page->order = 0;
    }

    // Get the first page of the block and update its order since it is
    // the head of the block.
    struct page *pg = page_info(pbase);
    pg->order = order;

    // Some sanity checks to ensure that the parameter is valid
    // and that the block has not been freed before. Those checks
    // are logic checks and should not happen in a normal execution.
    if ((base % PAGE_SIZE) != 0) {
        panic("buddy_free(): unaligned page address");
    } else if (buddy_initialized) {
        if (pg->flags & PG_RESERVED) {
            panic("buddy_free(): trying to free a reserved page");
        } else if (pg->flags & PG_POISONED) {
            panic("buddy_free(): trying to free a poisoned page");
        } else if (pg->flags & PG_FREE) {
            panic("buddy_free(): double free detected");
        }
    }

    // Coalesce the block with its buddy blocks until it is no longer
    // possible (i.e. the buddy block is not free, the maximum order
    // was reached...)
    while(buddy_can_coalesce(base)) {
        const vaddr buddy_base = buddy_address(base, pg->order);
        struct buddy_block *buddy = (struct buddy_block *) buddy_base;
        struct page *buddy_pg = page_info(buddy_vaddr_to_paddr(buddy_base));

        list_remove(&buddy->list);

        // Depending on the order of the buddy block, the base address of the
        // coalesced block will be the base address of the current block or the
        // base address of the buddy block.
        if (buddy_base < base) {
            pg->order = 0;
            base = buddy_base;
            pg = buddy_pg;
        }
        pg->order++;
    }

    // After coalescing, the block is now free and can be added to the free
    // list. We simply need to create a new buddy block structure at the base
    // address of the coalesced block and add it to the free list.
    struct buddy_block *block = create_buddy_block_at(base);
    list_add_head(&buddy_buckets[pg->order], &block->list);
}

/**
 * @brief Allocate a block of memory from the buddy allocator with the given
 * order. The order of the block determines the size of the block.
 * 
 * @param order The order of the block to allocate. It must be between
 * `BUDDY_MIN_ORDER` and `BUDDY_MAX_ORDER` inclusive. If the order is outside
 * this range, this function panics.
 * @return void* The base address of the allocated block, or NULL if the
 * allocation failed.
 */
void *buddy_alloc(u32 order)
{
    assert(order <= BUDDY_MAX_ORDER);

    // Find the first non-empty bucket with a block of at least
    // the given order. If no such block is found, return NULL.
    // If a block is found, remove it from the free list and
    // split it into smaller blocks until the desired order is
    // reached.
    for (u32 i = order; i <= BUDDY_MAX_ORDER; i++) {
        struct list_head *entry = list_pop_head(&buddy_buckets[i]);   
        struct buddy_block *block = list_entry(entry, struct buddy_block, list);

        // If the bucket is empty, continue to the next bucket. The block
        // will be found in a bucket with a higher order and will be split
        // into smaller blocks to reach the desired order.
        if (!entry) {
            continue;
        }
        
        // Split the block into smaller blocks until the desired order
        // is reached. The remaining blocks are added to the free list.
        for (u32 j = i; j > order; j--) {
            const vaddr base = buddy_address((vaddr) block, j - 1);
            struct buddy_block *buddy = create_buddy_block_at(base);
            list_add_head(&buddy_buckets[j - 1], &buddy->list);

            // Update the page information for the buddy block to reflect
            // the new order of the created block.
            struct page *pg = page_info(buddy_vaddr_to_paddr(base));
            pg->order = j - 1;
        }

        // Update the page information for the block (eventually splitted
        // into smaller blocks to avoid wasting too much memory) and return
        // the base address of the allocated block.
        paddr base = buddy_vaddr_to_paddr((vaddr) block);
        for (u32 j = 0; j < buddy_order_to_pfn(order); j++) {
            struct page *page = page_info(base + (j << PAGE_SHIFT));
            assert(!(page->flags & PG_RESERVED));
            assert(!(page->flags & PG_POISONED));
            assert(!(page->flags & PG_KERNEL));
            assert(page->flags & PG_FREE);
            page->flags &= ~PG_FREE;
            page->order = 0;
        }

        return block;
    }

    warn("buddy_alloc(): cannot allocate block of order %u", order);
    return NULL;
}

/**
 * @brief Allocate a block of memory from the buddy allocator with an exact
 * physical frame number. The block allocated will contain the given physical
 * 
 * @param pfn The physical frame number of the block to allocate.
 * @return void* The base address of the allocated block, or NULL if the
 * allocation failed.
 */
void *buddy_alloc_exact(uint pfn)
{
    // Find the order of the block that contains the given physical frame
    // number. The order is the smallest power of two that is greater than
    // or equal to the number of pages in the block.
    u32 order = 0;
    while (buddy_order_to_pfn(order) < pfn) {
        order++;
    }

    // Allocate a block of the given order, free extra pages, and return
    // the base address of the block.
    void *block = buddy_alloc(order);
    if (block == NULL) {
        return NULL;
    }

    for (u32 i = pfn; i < buddy_order_to_pfn(order); i++) {
        buddy_free((char *) block + page_pnf_to_offset(i), 1);
    }

    return block;
}

/**
 * @brief Free the exact block of memory allocated by the buddy allocator with
 * the given physical frame number. The block must have been allocated by the
 * buddy allocator and must not have been freed before. If the block has already
 * been freed, this function will panic.
 * 
 * @param ptr The base address of the block to free.
 * @param pfn The number of pages in the block to free.
 */
void buddy_free_exact(void *ptr, uint pfn)
{
    u32 order = 0;
    while (buddy_order_to_pfn(order + 1) < pfn) {
        order++;
    }

    buddy_free(ptr, order);
    for (u32 i = buddy_order_to_pfn(order); i < pfn; i++) {
        buddy_free((char *) ptr + page_pnf_to_offset(i), 1);
    }
}