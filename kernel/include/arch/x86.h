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
#include <arch/cpu.h>

typedef u32 paddr;
typedef u32 vaddr;

/**
 * @brief Align a physical address to the given alignment by rounding it up. If
 * the address is already aligned, it is returned as is and is not modified.
 * 
 * @param addr The address to align.
 * @param align The alignment to use, must be a power of 2.
 * @return paddr The aligned address.
 */
static inline paddr paddr_align_up(paddr addr, size_t align)
{
    return (addr + align - 1) & ~(align - 1);
}

/**
 * @brief Align a physical address to the given alignment by rounding it down.
 * If the address is already aligned, it is returned as is and is not modified.
 * 
 * @param addr The address to align.
 * @param align The alignment to use, must be a power of 2.
 * @return paddr The aligned address.
 */
static inline paddr paddr_align_down(paddr addr, size_t align)
{
    return addr & ~(align - 1);
}

/**
 * @brief Check if a physical address is aligned to the given alignment.
 * 
 * @param addr The address to check.
 * @param align The alignment to check, must be a power of 2.
 * @return true If the address is aligned to the given alignment.
 * @return false If the address is not aligned to the given alignment.
 */
static inline bool paddr_is_aligned(paddr addr, size_t align)
{
    return (addr & (align - 1)) == 0;
}

/**
 * @brief Align a virtual address to the given alignment by rounding it up. If
 * the address is already aligned, it is returned as is and is not modified.
 * 
 * @param addr The address to align.
 * @param align The alignment to use, must be a power of 2.
 * @return vaddr The aligned address.
 */
static inline vaddr vaddr_align_up(vaddr addr, size_t align)
{
    return (addr + align - 1) & ~(align - 1);
}

/**
 * @brief Align a virtual address to the given alignment by rounding it down.
 * If the address is already aligned, it is returned as is and is not modified.
 * 
 * @param addr The address to align.
 * @param align The alignment to use, must be a power of 2.
 * @return vaddr The aligned address.
 */
static inline vaddr vaddr_align_down(vaddr addr, size_t align)
{
    return addr & ~(align - 1);
}

/**
 * @brief Check if a virtual address is aligned to the given alignment.
 * 
 * @param addr The address to check.
 * @param align The alignment to check, must be a power of 2.
 * @return true If the address is aligned to the given alignment.
 * @return false If the address is not aligned to the given alignment.
 */
static inline bool vaddr_is_aligned(vaddr addr, size_t align)
{
    return (addr & (align - 1)) == 0;
}

/// The address of this symbol is the end of the kernel image in memory. The
/// address of this symbol is set by the linker script and DOES NOT CONTAIN
/// any meaningful data ! Only its address should be used.
extern char __end[];

void arch_x86_setup(struct mb_info *mb_info);
