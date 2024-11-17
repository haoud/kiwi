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

#define MB_HEADER_MAGIC			0x1BADB002
#define MB_BOOTLOADER_MAGIC		0x2BADB002
#define MB_STACK_SIZE			0x4000

#define MB_INFO_MEMORY                   0x00000001
#define MB_INFO_BOOTDEV                  0x00000002
#define MB_INFO_CMDLINE                  0x00000004
#define MB_INFO_MODS                     0x00000008
#define MB_INFO_AOUT_SYMS                0x00000010
#define MB_INFO_ELF_SHDR                 0x00000020
#define MB_INFO_MEMMAP                   0x00000040
#define MB_INFO_DRIVE_INFO               0x00000080
#define MB_INFO_CONFIG_TABLE             0x00000100
#define MB_INFO_BOOT_LOADER_NAME         0x00000200
#define MB_INFO_APM_TABLE                0x00000400
#define MB_INFO_VBE_INFO                 0x00000800
#define MB_INFO_FRAMEBUFFER_INFO         0x00001000

#define MB_MEMORY_AVAILABLE              1
#define MB_MEMORY_RESERVED               2
#define MB_MEMORY_ACPI_RECLAIMABLE       3
#define MB_MEMORY_NVS                    4
#define MB_MEMORY_BADRAM                 5

struct mb_header {
    u32 magic;
    u32 flags;
    u32 checksum;
    u32 header_addr;
    u32 load_addr;
    u32 load_end_addr;
    u32 bss_end_addr;
    u32 entry_addr;
} __attribute__((packed));

struct mb_elf_table {
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
} __attribute__((packed));

struct mb_module {
    u32 mod_start;
    u32 mod_end;
    u32 string;
    u32 reserved;
} __attribute__((packed));

struct mb_mmap {
    u32 size;
    u64 addr;
    u64 len;
    u32 type;
} __attribute__((packed));

struct mb_info {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;

    struct mb_elf_table elf_sec;

    u32 mmap_length;
    u32 mmap_addr;

    u32 drives_length;
    u32 drives_addr;
} __attribute__((packed));

/**
 * @brief Get the first memory map entry outside of the memory map provided by
 * the bootloader. This is useful to iterate over the memory map, but remember
 * 
 * @param mb_info The multiboot information structure.
 * @return struct mb_mmap* The end of the memory map. Using this pointer will
 * result in a out-of-bounds access.
 */
static inline struct mb_mmap *mb_mmap_end(struct mb_info *mb_info) {
    return (struct mb_mmap *) (mb_info->mmap_addr + mb_info->mmap_length);
}

/**
 * @brief Get the next memory map entry that follows the given entry. However,
 * the caller MUST ensure that the next entry is within the bounds of the
 * memory map (i.e. the address of the next entry is less than the address of
 * the end of the memory map). Failing to do so will result in undefined
 * behavior.
 * 
 * @param mmap The current memory map entry.
 * @return struct mb_mmap* The next memory map entry.
 */
static inline struct mb_mmap *mb_next_mmap(struct mb_mmap *mmap) {
    return (struct mb_mmap *) ((u32) mmap + mmap->size + sizeof(mmap->size));
}