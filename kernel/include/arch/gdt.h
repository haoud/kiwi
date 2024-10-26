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

#define GDT_MAX_ENTRIES     8

// Descriptor Privilege Level. Silicium only supports DLP 0 (kernel) and
// DLP 3 (user) and does not use the DLP 1 and 2.

#define GDT_KERNEL_DLP  0
#define GDT_USER_DLP    3

// Segment selectors

#define GDT_KERNEL_CS       0x08
#define GDT_KERNEL_DS       0x10
#define GDT_USER_CS         0x18
#define GDT_USER_DS         0x20
#define GDT_TSS             0x28

// Segment access flags for normal segments

#define GDT_ACCESSED        0x01
#define GDT_DATA_WRITABLE   0x02
#define GDT_DATA_GROW_DOWN  0x04
#define GDT_CODE_READABLE   0x02
#define GDT_CODE_CONFORMING 0x04
#define GDT_CODE_SEGMENT    0x08
#define GDT_SEGMENT_NORMAL  0x10
#define GDT_PRESENT         0x80

// Segment flags for all segments

#define GDT_LONG_MODE       0x02
#define GDT_FLAGS_32BITS    0x04
#define GDT_4KB_GRANULARITY 0x08

// Segment flags for system segments

#define GDT_TSS32_AVAILABLE   0x09

struct gdtr {
    u16 size;
    u32 base;
} __attribute__((packed));

struct gdt_entry {
    u16 limit0_15;
    u16 base0_15;
    u8 base16_23;
    u8 access;
    u8 limit16_19 : 4;
    u8 flags : 4;
    u8 base24_31;
} __attribute__((packed));

struct tss {
    u16 link, __link;
    u32 esp0;
    u16 ss0, __ss0;
    u32 esp1;
    u16 ss1, __ss1;
    u32 esp2;
    u16 ss2, __ss2;

    u32 cr3, eip, eflags;
    u32 eax, ecx, edx, ebx;
    u32 esp, ebp, esi, edi;

    u16 es, __es;
    u16 cs, __cs;
    u16 ss, __ss;
    u16 ds, __ds;
    u16 fs, __fs;
    u16 gs, __gs;
    u16 ldt, __ldt;
    u16 debug, io_map;
} __attribute__((packed));

void gdt_setup(void);
void gdt_set_tss_esp0(u32 esp);
