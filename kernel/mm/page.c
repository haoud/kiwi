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
#include <lib/log.h>
#include <lib/panic.h>
#include <mm/page.h>
#include <arch/x86.h>
#include <arch/paging.h>
#include <arch/serial.h>

/// The page array. This array contains information about each physical page
/// in the system, such as the page type (free, reserved, kernel, poisoned),
/// the number of references to the page, and other information.
static struct page *pages = NULL;

/// The number of reserved pages in the system. Reserved pages are pages
/// reserved by devices and does not contain RAM data. An example of reserved
/// pages are VGA memory, used by the VGA controller to store the screen
/// buffer.
static unsigned int pg_reserved = 0;

/// The number of poisoned pages in the system. Poisoned pages are pages
/// that cannot be used for any purpose, including allocation or device
/// mapping. They are either bad memory or memory that was not properly
/// included in the memory map.
static unsigned int pg_poisoned = 0;

/// The number of kernel pages in the system.
static unsigned int pg_kernel = 0;

/// The number of free pages in the system.
static unsigned int pg_free = 0;

/// The number of pages in the system. This is also the number of entries
/// in the `pages` array.
static unsigned int pg_count = 0;

/**
 * @brief Sanitize the memory map provided by the bootloader. Essentially, this
 * function will exclude the kernel memory from the free memory regions, because
 * the multiboot specification does not mark the kernel memory as used in the
 * memory map (and it is fucking stupid to be honest).
 * 
 * Since the kernel is loaded at the first megabyte of memory, we can safely
 * assume that the kernel base address should be the base address of a free
 * memory region since some memory below the kernel is reserved for the BIOS.
 * This greatly simplifies the process of finding the kernel memory in the
 * memory map, but if this is not the case, this function will panic.
 * 
 * @param mb_info The multiboot information structure.
 */
static void sanitize_mmap(struct mb_info *mb_info) 
{
    struct mb_mmap *mmap = (struct mb_mmap *) mb_info->mmap_addr;
    bool kernel_found = false;

    while (mmap < mb_mmap_end(mb_info)) {
        if (mmap->type == MB_MEMORY_AVAILABLE) {
            const vaddr kernel_start = KERNEL_VBASE + 0x100000;
            const vaddr kernel_end = vaddr_align_up((vaddr) &__end, PAGE_SIZE);
            const u32 kernel_size = kernel_end - kernel_start;

            if (mmap->addr == KERNEL_PBASE) {
                mmap->addr += kernel_size;
                mmap->len -= kernel_size;
                kernel_found = true;
            }
        }
        mmap = mb_next_mmap(mmap);
    }

    if (!kernel_found) {
        panic("Kernel memory not found in memory map");
    }
}


/**
 * @brief Find the last regular address in the memory map and return it. This
 * is useful to calculate the size of the page array to avoid wasting memory
 * by allocating a overly large page array.
 * 
 * @param mb_info The loaded multiboot information structure.
 * @return paddr The last regular address in the memory map, or 0 if no
 * regular addresses were found.
 */
static paddr find_last_regular_address(struct mb_info *mb_info)
{
    struct mb_mmap *mmap = (struct mb_mmap *) mb_info->mmap_addr;
    paddr last = 0;

    while (mmap < mb_mmap_end(mb_info)) {
        if (mmap->type == MB_MEMORY_AVAILABLE) {
            last = (u32) (mmap->addr + mmap->len - 1);
        }
        mmap = mb_next_mmap(mmap);
    }

    return last;
}

/**
 * @brief Allocate memory during the boot process by modifying the memory map
 * provided by the bootloader. The allocated memory will be deducted from the
 * memory map.
 * 
 * @param mb_info The loaded multiboot information structure.
 * @return void* The pointer to the allocated memory, or NULL if the memory
 * could not be allocated.
 */
static void *allocate_boot_memory(struct mb_info *mb_info, size_t size)
{
    struct mb_mmap *mmap = (struct mb_mmap *) mb_info->mmap_addr;
    struct mb_mmap *free_mmap = NULL;
    const u32 align = 16;

    while (mmap < mb_mmap_end(mb_info)) {
        if (mmap->type == MB_MEMORY_AVAILABLE && mmap->len >= size + align) {
            free_mmap = mmap;
        }
        mmap = mb_next_mmap(mmap);
    }

    // If we did not find a suitable free memory region, return NULL
    if (free_mmap == NULL) {
        return NULL;
    }

    // Calculate the number of bytes needed to align the memory
    const u32 aligned_area_base = paddr_align_up(free_mmap->addr, align);
    const u32 misalign = aligned_area_base - free_mmap->addr;

    // Update the memory map
    free_mmap->addr = aligned_area_base + size;
    free_mmap->len -= size + misalign;

    return (void *) KERNEL_VBASE + aligned_area_base;
}

/**
 * @brief Change the type of a page and update page counters accordingly.
 * 
 * @param pg The page to change the type of.
 * @param type The new type of the page. This should be one of the following
 * values: PG_FREE, PG_RESERVED, PG_POISONED, PG_KERNEL.
 */
static void page_change_type(struct page *pg, int type)
{
    if (pg->flags & PG_FREE) {
        pg_free--;
    } else if (pg->flags & PG_KERNEL) {
        pg_kernel--;
    } else if (pg->flags & PG_RESERVED) {
        pg_reserved--;
    } else if (pg->flags & PG_POISONED) {
        pg_poisoned--;
    } 

    if (type == PG_FREE) {
        pg_free++;
    } else if (type == PG_KERNEL) {
        pg_kernel++;
    } else if (type == PG_RESERVED) {
        pg_reserved++;
    } else if (type == PG_POISONED) {
        pg_poisoned++;
    } else {
        panic("page_change_type(): Invalid page type");
    }

    pg->flags = type;
}

/**
 * @brief Setup the page array and mark pages as free, reserved, or kernel
 * memory based on the memory map provided by the bootloader.
 * 
 * @param mb_info The multiboot information structure.
 */
_init
void page_setup(struct mb_info *mb_info)
{
    if (!(mb_info->flags & MB_INFO_MEMMAP)) {
        // TODO: Implement a sort of memory probing
        // The problem is that it is slow and not very reliable and
        // can potentially permanently damage the system if we write
        // to device-mapped memory. Therefore, we should only do this
        // if we have no other choice, and do it very conservatively.
        panic("No memory map provided by the bootloader");
    }

    sanitize_mmap(mb_info);

    paddr pg_last = find_last_regular_address(mb_info);
    if (!pg_last) {
        panic("Unable to find last regular address in memory map");
    }

    pg_count = page_idx(paddr_align_up(pg_last, PAGE_SIZE));
    pages = allocate_boot_memory(mb_info, pg_count * sizeof(struct page));
    if (pages == NULL) {
        panic("Unable to allocate memory for page array");
    }

    debug("Page array at %08x (%u pages)", pages, pg_count);

    // Initialize the page array and poison all pages by default. During the
    // page setup process, we will mark pages as free, reserved, or kernel
    // memory, and any pages that are not marked will be considered poisoned
    // and unusable to avoid any potential issues.
    pg_poisoned = pg_count;
    for (u32 i = 0; i < pg_count; i++) {
        pages[i].flags = PG_POISONED;
        pages[i].count = 0;
    }

    // Use the memory map to mark pages as free or reserved
    struct mb_mmap *mmap = (struct mb_mmap *) mb_info->mmap_addr;
    while (mmap < mb_mmap_end(mb_info)) {
        if (mmap->type == MB_MEMORY_AVAILABLE) {
            const u32 start = page_idx(mmap->addr);
            const u32 end = page_idx(mmap->addr + mmap->len);
            for (u32 i = start; i < end; i++) {
                page_change_type(&pages[i], PG_FREE);
            }
        } else if (mmap->type == MB_MEMORY_RESERVED) {
            const u32 start = page_idx(mmap->addr);
            const u32 end = page_idx(mmap->addr + mmap->len);
            for (u32 i = start; i < end; i++) {
                page_change_type(&pages[i], PG_RESERVED);
            }
        }
        mmap = mb_next_mmap(mmap);
    }

    // The first page of memory is reserved by the BIOS. Furthermore, since 
    // the first page is never valid, we can safely return it to indicate an
    // error (el famoso 1 billion dollar mistake).
    page_change_type(&pages[0], PG_RESERVED);

    // Reserve the area used by the BIOS and some other devices
    const u32 bios_start_idx = page_idx(0xA0000);
    const u32 bios_end_idx = page_idx(0x100000);
    for (u32 i = bios_start_idx; i < bios_end_idx; i++) {
        page_change_type(&pages[i], PG_RESERVED);
    }
    
    // Mark the kernel pages as used by the kernel
    const u32 kernel_end_paddr = (vaddr) &__end - KERNEL_VBASE;
    const u32 kernel_start_idx = page_idx(KERNEL_PBASE);
    const u32 kernel_end_idx = page_idx(kernel_end_paddr);
    for (u32 i = kernel_start_idx; i < kernel_end_idx; i++) {
        page_change_type(&pages[i], PG_KERNEL);
        pages[i].count = 1;
    }

    // Mark the page array as used by the kernel
    const u32 page_array_start_idx = page_idx((paddr) ((vaddr) pages - KERNEL_VBASE));
    const u32 page_array_end_idx = page_idx((paddr) &pages[pg_count] - KERNEL_VBASE);
    for (u32 i = page_array_start_idx; i < page_array_end_idx; i++) {
        page_change_type(&pages[i], PG_KERNEL);
        pages[i].count = 1;
    }

    const u32 pg_free_kib = pg_free * 4;
    const u32 pg_kernel_kib = pg_kernel * 4;
    const u32 pg_reserved_kib = pg_reserved * 4;
    const u32 pg_poisoned_kib = pg_poisoned * 4;

    debug("Free pages: %u (%u KiB)", pg_free, pg_free_kib);
    debug("Reserved pages: %u (%u KiB)", pg_reserved, pg_reserved_kib);
    debug("Poisoned pages: %u (%u KiB)", pg_poisoned, pg_poisoned_kib);
    debug("Kernel pages: %u (%u KiB)", pg_kernel, pg_kernel_kib);
}