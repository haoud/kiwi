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
#include <multiboot.h>
#include <arch/x86.h>

#define PAGE_SIZE 4096

#define PG_FREE     0x01
#define PG_KERNEL   0x02
#define PG_RESERVED 0x04
#define PG_POISONED 0x08
#define PG_LOCKED   0x10

/**
 * @brief Check if a physical address is compatible with the BIOS, i.e. if it
 * is in the first megabyte of memory and can be used/accessed by the BIOS.
 * 
 * @param addr The physical address.
 * @return true if the address is compatible with the BIOS.
 * @return false if the address is not compatible with the BIOS.
 */
static inline bool page_bios_compatible(paddr addr) {
    return addr < 0x100000;
}

/**
 * @brief Check if a physical address is compatible with the ISA bus, i.e. if it
 * is in the first megabyte of memory and can be used/accessed by ISA devices.
 * 
 * @param addr The physical address.
 * @return true if the address is compatible with the ISA bus.
 * @return false if the address is not compatible with the ISA bus.
 */
static inline bool page_isa_compatible(paddr addr) {
    return addr < 0x100000;
}

/**
 * @brief Check if a physical address is compatible with low memory, i.e. if it
 * is in the first 512 megabytes of memory and can directly accessed by the 
 * kernel without the need for temporary mappings.
 * 
 * @param addr The physical address.
 * @return true if the address is compatible with low memory.
 * @return false if the address is not compatible with low memory.
 */
static inline bool page_lowmem_compatible(paddr addr) {
    return addr < 0x20000000;
}

/**
 * @brief Get the page index of a physical address.
 * 
 * @param addr The physical address.
 * @return u32 The page index.
 */
static inline u32 page_idx(paddr addr) {
    return addr / PAGE_SIZE;
}

struct page {
    u16 flags;
    u16 count;
};

void page_setup(struct mb_info *mb_info);