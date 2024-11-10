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
#pragma once
#include <kernel.h>
#include <arch/x86.h>
#include <lib/assert.h>

#define KERNEL_VBASE    0xC0000000 
#define KERNEL_MAX_PAGE 0x40000000

struct pde {
    union {
        struct {
            u32 present : 1;
            u32 rw : 1;
            u32 user : 1;
            u32 write_through : 1;
            u32 cache_disabled : 1;
            u32 accessed : 1;
            u32 dirty : 1;
            u32 page_size : 1;
            u32 global : 1;
            u32 available : 3;
            u32 frame : 20;
        };
        u32 v;
    };
} __attribute__((packed));

struct pte {
    union {
        struct {
            u32 present : 1;
            u32 rw : 1;
            u32 user : 1;
            u32 write_through : 1;
            u32 cache_disabled : 1;
            u32 accessed : 1;
            u32 dirty : 1;
            u32 pat : 1;
            u32 global : 1;
            u32 available : 3;
            u32 frame : 20;
        };
        u32 v;
    };
} __attribute__((packed));

struct page_directory {
    struct pde entries[1024];
} __attribute__((packed, aligned(4096)));

struct page_table {
    struct pte entries[1024];
} __attribute__((packed, aligned(4096)));

/**
 * @brief Convert a physical address to a virtual address. This function
 * assumes that all the physical addresses usable by the kernel are mapped
 * to the virtual address space starting at KERNEL_VBASE. If the physical
 * address is not mapped inside the kernel space (i.e. it is greater than
 * KERNEL_MAX_PAGE), this function will panic.
 * 
 * @param paddr The physical address to convert to a virtual address, must be
 * less than KERNEL_MAX_PAGE
 * @return vaddr_t The virtual address corresponding to the physical address
 */
_const
static inline vaddr_t paddr_to_vaddr(paddr_t paddr) {
    assert(paddr < KERNEL_MAX_PAGE);
    return KERNEL_VBASE + paddr;
}

void paging_setup(void);