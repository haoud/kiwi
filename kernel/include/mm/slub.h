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
#include <mm/page.h>
#include <lib/list.h>
#include <arch/paging.h>

#define SLUB_MIN_SIZE   8       // Minimum size of an object
#define SLUB_MIN_ALIGN  8       // Minimum alignment of an object
#define SLUB_DEFAULT_ORDER  2   // Default page order for slub size

#define SLUB_MAX_OBJ_COUNT  UINT16_MAX  // Maximum number of objects per slub
#define SLUB_MAX_OBJ_SIZE   UINT16_MAX  // Maximum size of an object
#define SLUB_MAX_ALIGN      PAGE_SIZE   // Maximum alignment of an object

/// @brief No flags set for the slub cache.
#define SLUB_NONE       0x00

/// @brief When set, slub with this flag will not be freed when 
/// become empty. This can be useful for caches that are staticaly
/// allocated and should not be freed.
#define SLUB_STICKY     0x01

/// @brief When set, slub with this flag will print debug information
/// when allocating and freeing objects.
#define SLUB_DEBUG      0x02

/**
 * @brief The structure representing a slub cache. A slub cache is a collection
 * of slubs that are used to allocate objects of a specific size. Slub caches
 * are used to improve the performance of memory allocation by reducing the
 * overhead of allocating and freeing objects, and by improving the cache
 * locality of the program.
 */
struct slub_cache {
    /// @brief The name of the cache, used for debugging purposes.
    const char *name;

    /// @brief A set of flags that control the behavior of the cache.
    uint flags;

    /// @brief The order of the slub size, in pages.
    u16 order;

    /// @brief The number of total objects that the cache can hold.
    u32 total_obj_count;

    /// @brief The number of free objects in the cache.
    u32 free_obj_count;

    /// @brief The minimum alignment of the objects in the cache, in bytes.
    u16 obj_align;

    /// @brief The size of the objects that will be allocated from the cache,
    /// in bytes.
    u16 obj_size;

    /// @brief The number of objects that can be allocated from a single slub.
    u16 obj_per_slub;

    /// @brief The minimum number of free objects that the cache must contain
    /// before allocating a new slub. This is particularly useful to avoid
    /// infinite recursion when allocating new slubs from the slub cache, that
    /// also requires a slub to be allocated !
    u16 min_free;

    /// @brief A list of slubs that are free does not contain any allocated
    /// objects. These slubs are ready to be used for new allocations, but
    /// should only be used if there are no partial slubs available.
    struct list_head free_slubs;

    /// @brief A list of slubs that contain at least one allocated object.
    /// Allocations will be made from these slubs until they become full.
    struct list_head partial_slubs;

    /// @brief A list of slubs that are full and do not contain any free
    /// objects. These slubs cannot be used for new allocations until at least
    /// one object is freed from them.
    struct list_head full_slubs;
};

/**
 * @brief The structure representing a slub. A slub is a region of memory that
 * is used to allocate objects of a specific size. Slubs are used by slub caches
 * to allocate and free objects efficiently.
 */
struct slub {
    /// @brief The base virtual address of the slub, where the objects are
    /// allocated.
    vaddr base;

    /// @brief The size of the slub in bytes.
    size_t size;

    /// @brief  The maximum number of objects that can be allocated in the
    /// slub.
    u16 max_objects;

    /// @brief The number of free objects in the slub.
    u16 free_objects;

    /// @brief The cache that this slub belongs to.
    struct slub_cache *cache;

    /// @brief A list node to link slub objects together in the cache.
    struct list_head slub_node;

    /// @brief A list to link free objects together in the slub.
    struct list_head free_obj_list;
};

void slub_setup(void);
void slub_free(struct slub_cache *cache, void *ptr);
void *slub_alloc(struct slub_cache *cache);
void slub_destroy_cache(struct slub_cache *cache);
struct slub_cache *slub_create_cache(
    const char *name,
    u16 obj_size,
    u16 obj_align,
    u16 min_free,
    uint flags);
