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
#include <lib/math.h>
#include <multiboot.h>
#include <arch/cpu.h>

typedef u32 paddr;
typedef u32 vaddr;

#define page_align_up(a)    align_up(a, PAGE_SIZE)
#define page_align_down(a)  align_down(a, PAGE_SIZE)
#define page_is_aligned(a)  is_aligned(a, PAGE_SIZE)

/// The address of this symbol is the end of the kernel image in memory. The
/// address of this symbol is set by the linker script and DOES NOT CONTAIN
/// any meaningful data ! Only its address should be used.
extern char __end[];

void arch_x86_setup(struct mb_info *mb_info);
