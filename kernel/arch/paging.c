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
#include <arch/paging.h>

// The kernel page directory, defined in arch/asm/boot.asm
extern struct page_directory kernel_pd;

/**
 * @brief Setup the paging system. Most of the work is done in the boot.asm
 * file, needed to setup an higher-half kernel. Here, we just need to unmap
 * the first 4MB of memory, used briefly by the bootloader when setting up
 * the paging system.
 */
_init
void paging_setup() {
    for (int i = 0; i < 768; i++) {
        kernel_pd.entries[i].v = 0;
    }
}