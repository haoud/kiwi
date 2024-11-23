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
#include <lib/list.h>

/// @brief The minimum order of a block in the buddy allocator (4 Kib blocks).
#define BUDDY_MIN_ORDER 1

/// @brief The maximum order of a block in the buddy allocator (64 MiB blocks).
#define BUDDY_MAX_ORDER 15

/// @brief The maximum number of pages that the buddy allocator can manage.
#define BUDDY_MAX_PAGES ((512 * 1024 * 1024) / PAGE_SIZE) 

/// @brief The number of buckets in the buddy allocator.
#define BUDDY_BUCKET_COUNT BUDDY_MAX_ORDER

struct buddy_block {
    struct list_head list;
};

void buddy_setup(void);
void buddy_debug(void);
void buddy_free(void *ptr, u32 order);
void *buddy_alloc(u32 order);
