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
#include <mm/slub.h>
#include <mm/malloc.h>

struct malloc_cache {
    struct slub_cache *cache;
    size_t size;
};

/// @brief A set of caches for different object sizes, used by the malloc()
/// function to allocate objects of different sizes.
static struct malloc_cache caches[] = {
    {NULL, 16},
    {NULL, 32},
    {NULL, 64},
    {NULL, 128},
    {NULL, 256},
    {NULL, 512},
    {NULL, 1024},
    {NULL, 2048},
    {NULL, 4096},
};

/**
 * @brief Setup the malloc() function by creating a set of slub caches for
 * different object sizes. Objects that does not exactly match the size of the
 * caches will be allocated from the closest cache that is larger than the
 * requested size.
 */
_init
void malloc_setup()
{
    for (size_t i = 0; i < 9; i++) {
        caches[i].cache = slub_create_cache(
            "malloc", caches[i].size, 0, 0, SLUB_NONE);

        if (caches[i].cache == NULL) {
            panic("Failed to create malloc cache for size %u", caches[i].size);
        }
    }
}

/**
 * @brief Allocate a new object of the specified size, with a guaranteed
 * alignment of 8 bytes.
 * 
 * @param size The size of the object to allocate.
 * @return void* A pointer to the allocated object, or NULL if the allocation
 * failed.
 */
void *malloc(size_t size)
{
    for (size_t i = 0; i < 9; i++) {
        if (size <= caches[i].size) {
            return slub_alloc(caches[i].cache);
        }
    }

    warn("malloc() does not support allocation larger than a page");
    return NULL;
}

/**
 * @brief Free an object allocated with malloc().
 * 
 * @param ptr The pointer to the object to free. If the pointer is NULL, the
 * function does nothing.
 */
void free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    for (size_t i = 0; i < 9; i++) {
        slub_free(caches[i].cache, ptr);
    }
}