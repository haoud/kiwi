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
#include <kernel.h>
#include <arch/cpu.h>

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed));

struct gdtr {
    u16 limit;
    u32 base;
} __attribute__((packed));

static struct gdt_entry gdt[5];
static struct gdtr gdtr;

/**
 * @brief Set a GDT descriptor to the specified values
 *
 * @param i The index of the descriptor. It must be between 0 and 4
 * @param base The base address of the segment
 * @param limit The limit of the segment
 * @param access Segment access flags
 * @param granularity Segment granularity flags
 */
static void gdt_set_descriptor(
    u32 i,
    u64 base,
    u64 limit,
    u8 access,
    u8 granularity)
{
    gdt[i].base_low = (base & 0xFFFF);
    gdt[i].base_middle = (base >> 16) & 0xFF;
    gdt[i].base_high = (base >> 24) & 0xFF;

    gdt[i].limit_low = (limit & 0xFFFF);

    gdt[i].granularity = (limit >> 16) & 0x0F;
    gdt[i].granularity |= granularity & 0xF0;

    gdt[i].access = access;
}

/**
 * @brief Setup a very basic GDT for the kernel
 */
void gdt_install(void)
{
    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (u32) &gdt;

    gdt_set_descriptor(0, 0, 0, 0, 0);
    gdt_set_descriptor(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_descriptor(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_descriptor(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_descriptor(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    asm volatile("lgdt %0" : : "m" (gdtr));
    asm volatile("      \n\
        mov ax, 0x10    \n\
        mov ds, ax      \n\
        mov es, ax      \n\
        mov fs, ax      \n\
        mov gs, ax      \n\
        mov ss, ax      \n\
        ljmp 0x08:next  \n\
        next:");
}
