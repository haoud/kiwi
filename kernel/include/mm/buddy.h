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
#define BUDDY_MIN_ORDER 0

/// @brief The maximum order of a block in the buddy allocator (64 MiB blocks).
#define BUDDY_MAX_ORDER 14

/// @brief The maximum number of pages that the buddy allocator can manage.
#define BUDDY_MAX_PAGES ((512 * 1024 * 1024) / PAGE_SIZE) 

/// @brief The number of buckets in the buddy allocator.
#define BUDDY_BUCKET_COUNT BUDDY_MAX_ORDER + 1

struct buddy_block {
    struct list_head list;
};

/**
 * @brief Get the nearest order of a block that can contain the given number of
 * pages. However, the order returned is not garanteed to be supported by the
 * buddy allocator. The order is the smallest power of two that is greater than
 * or equal to the number of pages.
 * 
 * @param pages The number of pages that the block must contain.
 * @return uint The order of the block that can contain the given number
 * of pages. If the number of pages is zero or one, the order will be zero.
 */
static inline uint buddy_nearest_order(uint pages) {
    if (pages <= 1 ) {
        return 0;
    } else {
        return 32 - __builtin_clz(pages - 1);
    }
}

/**
 * @brief Get the number of pages in the given order block.
 * 
 * @param order The order of the block. 
 * @return u32 The number of pages in the block.
 */
static inline u32 buddy_order_to_pfn(u32 order) {
    return 1 << order;
}

/**
 * @brief Get the number of bytes in the given order block.
 * 
 * @param order The order of the block.
 * @return u32 The number of bytes in the block.
 */
static inline u32 buddy_order_to_bytes(u32 order) {
    return buddy_order_to_pfn(order) * PAGE_SIZE;
}

void buddy_setup(void);
void buddy_debug(void);
void buddy_free(void *ptr, u32 order);
void *buddy_alloc(u32 order);
