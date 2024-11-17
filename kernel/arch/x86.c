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
#include <arch/gdt.h>
#include <arch/trap.h>
#include <arch/serial.h>
#include <arch/paging.h>
#include <arch/console.h>

/**
 * @brief Initialize the x86 architecture-specific components, such
 * as the GDT, IDT, and serial port.
 */
_init
void arch_x86_setup(struct mb_info *mb_info)
{
    console_setup();
    serial_setup();

    // If no multiboot information is provided, panic because we need it
    // to properly initialize the system
    if (mb_info == NULL) {
        panic("No multiboot information provided");
    }

    // Print memory information if available
    if (mb_info->flags & MB_INFO_MEMORY) {
        debug("Memory detected: %d KB",
            mb_info->mem_lower + mb_info->mem_upper);
    }

    // Print memory map if available
    if (mb_info->flags & MB_INFO_MEMMAP) {
        mb_info->mmap_addr += KERNEL_VBASE;
        struct mb_mmap *mmap = (struct mb_mmap *) mb_info->mmap_addr;
        const char *const MB_MEMORY_TYPES[] = {
            "Unknown",
            "Available",
            "Reserved",
            "ACPI Reclaimable",
            "ACPI NVS",
            "Bad RAM"
        };

        while (mmap < mb_mmap_end(mb_info)) {
            const u32 length = mmap->len & 0xFFFFFFFF;
            const u32 base = mmap->addr & 0xFFFFFFFF;
            const u32 end = base + (length + - 1);

            debug("Memory region: 0x%08x - 0x%08x (%s)",
                base, end, MB_MEMORY_TYPES[mmap->type]);
            mmap = mb_next_mmap(mmap);
        }
    }

    gdt_setup();
    trap_setup();
    paging_setup();
}
