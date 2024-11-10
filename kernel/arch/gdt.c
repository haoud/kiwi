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
#include <memory.h>
#include <arch/gdt.h>

static struct gdt_entry GDT[GDT_MAX_ENTRIES] = {};
static struct gdtr GDTR = {
    .size = sizeof(GDT) - 1,
    .base = (u32) GDT
};
static struct tss TSS = {};

/**
 * @brief Set a GDT descriptor. This function does not work for system
 * descriptors like TSS, use `gdt_set_system_descriptor` instead.
 * 
 * @param index The index of the descriptor. Must be less than GDT_MAX_ENTRIES.
 * @param base  The base address of the segment
 * @param limit The limit of the segment
 * @param ring The ring of the segment: it should be either GDT_KERNEL_DLP for
 * kernel segments or GDT_USER_DLP for user segments.
 * @param access The access flags of the segment. The `GDT_PRESENT` flag is
 * automatically set by this function to avoid mistakes.
 */
void gdt_set_descriptor(int index, u32 base, u32 limit, u8 ring, u8 access)
{
    struct gdt_entry *entry = &GDT[index];

    entry->base0_15 = base & 0xFFFF;
    entry->base16_23 = (base >> 16) & 0xFF;
    entry->base24_31 = (base >> 24) & 0xFF;
    entry->limit0_15 = limit & 0xFFFF;
    entry->limit16_19 = (limit >> 16) & 0x0F;
    entry->access = GDT_PRESENT | GDT_SEGMENT_NORMAL | access | (ring << 5);
    entry->flags = GDT_FLAGS_32BITS | GDT_4KB_GRANULARITY;
}

/**
 * @brief Set a system descriptor. This function is used to set TSS descriptors.
 * 
 * @param index The index of the descriptor. Must be less than GDT_MAX_ENTRIES.
 * @param tss A pointer to the TSS structure. The TSS structure must be valid
 * and must be accessible from the kernel while loaded in the GDT.
 */
void gdt_set_system_descriptor(int index, struct tss *tss)
{
    struct gdt_entry *entry = &GDT[index];

    entry->base0_15 = (u32) tss & 0xFFFF;
    entry->base16_23 = ((u32) tss >> 16) & 0xFF;
    entry->base24_31 = ((u32) tss >> 24) & 0xFF;
    entry->limit0_15 = sizeof(struct tss) & 0xFFFF;
    entry->limit16_19 = (sizeof(struct tss) >> 16) & 0x0F;
    entry->access = GDT_PRESENT | GDT_TSS32_AVAILABLE;
    entry->flags = GDT_FLAGS_32BITS;
}

/**
 * @brief Setup the GDT, load it into the processor and reload the segment
 * registers. If we are still here after this function, the GDT is correctly
 * set up (hopefully).
 */
_init
void gdt_setup(void)
{
    // Set the GDT descriptors
    gdt_set_descriptor(1, 0, 0xFFFFFFFF, GDT_KERNEL_DLP, GDT_CODE_SEGMENT);
    gdt_set_descriptor(2, 0, 0xFFFFFFFF, GDT_KERNEL_DLP, GDT_DATA_WRITABLE);
    gdt_set_descriptor(3, 0, 0xFFFFFFFF, GDT_USER_DLP, GDT_CODE_SEGMENT);
    gdt_set_descriptor(4, 0, 0xFFFFFFFF, GDT_USER_DLP, GDT_DATA_WRITABLE);

    // Setup the TSS
    gdt_set_system_descriptor(5, &TSS);
    TSS.iobp = sizeof(struct tss);
    TSS.ss0 = GDT_KERNEL_DS;

    // Load the GDT and the TSS and reload the segment registers
    asm volatile("lgdt %0" : : "m" (GDTR));
    asm volatile("ltr %0" : : "a" (GDT_TSS));
    asm volatile("mov ax, %0        \n\
                  mov ss, ax        \n\
                  mov ds, ax        \n\
                  mov es, ax        \n\
                  mov fs, ax        \n\
                  mov gs, ax        \n\
                  jmp 0x08:1f       \n\
                1:" : : "i" (GDT_KERNEL_DS));
}

/**
 * @brief Set the ESP0 field of the TSS. This function is used to set the
 * stack pointer of the kernel when the processor switches to ring 0.
 * 
 * @param esp The stack pointer to set.
 */
void gdt_set_tss_esp0(u32 esp)
{
    TSS.esp0 = esp;
}
