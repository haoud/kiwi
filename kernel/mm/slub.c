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
#include <mm/buddy.h>

#define align_up(x, a) (((x) + (a) - 1) & ~((a) - 1))

static struct slub_cache slub_cache_cache = { };
static struct slub_cache slub_cache = { };
static struct slub slub_cache_slub = { };
static struct slub slub_slub = { };

/**
 * @brief Check if the given pointer is contained within the slub region. 
 * However, this function does not check if the pointer is actually a valid
 * object in the slub.
 * 
 * @param slub The slub to check.
 * @param ptr The pointer to check.
 * @return true if the pointer is contained within the slub region
 * @return false if the pointer is not contained within the slub region.
 */
static bool slub_contains(struct slub *slub, void *ptr)
{
    return (vaddr) ptr >= slub->base &&
           (vaddr) ptr < slub->base + slub->size;
}

/**
 * @brief Add the given object to the free list of the slub.
 * 
 * @param slub The slub to add the object to.
 * @param object The object to add to the free list.
 */
static void slub_add_to_free_list(struct slub *slub, void *obj)
{
    struct list_head *node = (struct list_head *) obj;
    list_init(node);
    list_add_tail(&slub->free_obj_list, node);
}

/**
 * @brief Initialize the slub cache structure with the given parameters.
 * 
 * @param cache The cache to initialize.
 * @param name The name of the cache (for debugging purposes).
 * @param obj_size The size of the objects that will be allocated from the
 * cache, in bytes. If the size is less than SLUB_MIN_SIZE, the object size
 * will be set to SLUB_MIN_SIZE.
 * @param obj_align The alignment of the objects in the cache. It must be a
 * power of two, and less than SLUB_MAX_ALIGN. If zero or less than
 * SLUB_MIN_ALIGN, the default alignment of 8 bytes will be used.
 * @param flags A set of flags that control the behavior of the cache.
 */
static void slub_new_cache(
    struct slub_cache *cache,
    const char *name,
    u16 obj_size,
    u16 obj_align,
    u16 min_free,
    uint flags)
{
    if (obj_size < SLUB_MIN_ALIGN) {
        obj_size = SLUB_MIN_ALIGN;
    }
    if (obj_align < SLUB_MIN_ALIGN) {
        obj_align = SLUB_MIN_ALIGN;
    }

    cache->name = name;
    cache->flags = flags;
    cache->obj_align = obj_align;
    cache->obj_size = obj_size;
    cache->min_free = min_free;

    cache->total_obj_count = 0;
    cache->free_obj_count = 0;
    cache->obj_per_slub = SLUB_OBJ_COUNT;

    list_init(&cache->partial_slubs);
    list_init(&cache->free_slubs);
    list_init(&cache->full_slubs);
}

/**
 * @brief Construct a new slub for the given cache. This function initializes
 * the slub with the given parameters and adds it to the free slubs list of the
 * cache.
 * 
 * @param cache The cache that the slub belongs to.
 * @param slub The slub to initialize. 
 * @param base The base virtual address of the slub, where the objects are
 * allocated.
 * @param size The size of the slub in bytes. If the size is not a multiple of
 * the object size, the remaining space will be wasted.
 */
static void slub_new_slub(
    struct slub_cache *cache,
    struct slub *slub,
    vaddr base,
    size_t size)
{
    const uint max_objects = size / cache->obj_size;
    assert(max_objects <= SLUB_MAX_OBJ_COUNT);

    slub->cache = cache;
    slub->base = base;
    slub->size = size;
    slub->max_objects = (u16) max_objects;
    slub->free_objects = (u16) max_objects;

    list_init(&slub->slub_node);
    list_init(&slub->free_obj_list);
    list_add_tail(&cache->free_slubs, &slub->slub_node);

    // Create the free object list for the slub.
    for (u16 i = 0; i < max_objects; i++) {
        void *obj = (void *) align_up(
            base + i * cache->obj_size, cache->obj_align);
        slub_add_to_free_list(slub, obj);
    }
}

/**
 * @brief Add a new slub to the given cache. This function will allocate a new
 * slub from the buddy allocator and add it to the cache's free slubs list.
 * 
 * @param cache The cache to add the slub to.
 * @return true if the slub was successfully added to the cache.
 * @return false if the slub could not be added to the cache (likely due to an
 * out-of-memory condition).
 */
static bool slub_add_slub(struct slub_cache *cache)
{
    struct slub *slub = slub_alloc(&slub_cache);
    if (slub == NULL) {
        return false;
    }

    size_t size = cache->obj_size * cache->obj_per_slub;
    uint pfn = vaddr_align_up(size, PAGE_SIZE) / PAGE_SIZE;
    void *base = buddy_alloc_exact(pfn);
    if (base == NULL) {
        slub_free(&slub_cache, slub);
        return false;
    }

    // Create the new slub and add it to the free slubs list of the cache.
    slub_new_slub(cache, slub, (vaddr) base, size);
    return true;
}

/**
 * @brief Initialize the slub allocator. This function creates the cache for
 * allocating slub caches, and creates the cache for allocating slubs. After
 * calling this function, the slub allocator is ready to be used.
 */
_init
void slub_setup(void)
{
    vaddr slub_cache_slub_mem = (vaddr) buddy_alloc(4);
    vaddr slub_slub_mem = (vaddr) buddy_alloc(4);

    if (!slub_cache_slub_mem || !slub_slub_mem) {
        panic("Failed to allocate memory for slub caches");
    }

    // Create the cache for allocating slub caches and manually allocate the
    // first slub since the slub cache is not yet available.
    slub_new_cache(&slub_cache_cache, "slub cache", sizeof(struct slub_cache),
                   0, 1, SLUB_NONE);
    slub_new_slub(&slub_cache_cache, &slub_cache_slub,
                  slub_cache_slub_mem, 4 * PAGE_SIZE);

    // Create the cache for allocating slubs and manually add the first slub
    // to the cache since the slub cache is not yet available.
    slub_new_cache(&slub_cache, "slub", sizeof(struct slub), 0, 1, SLUB_NONE);
    slub_new_slub(&slub_cache, &slub_slub, slub_slub_mem, 4 * PAGE_SIZE);
}

/**
 * @brief Free an object that was previously allocated from a cache. If the
 * object was not allocated from the given cache, this function will simply
 * do nothing.
 * 
 * @param obj A pointer to the object to free.
 */
void slub_free(struct slub_cache *cache, void *obj)
{
    list_foreach_safe(&cache->partial_slubs, node) {
        struct slub *slub = list_entry(node, struct slub, slub_node);
        if (slub_contains(slub, obj)) {
            slub_add_to_free_list(slub, obj);
            cache->free_obj_count++;
            slub->free_objects++;
            
            // If the slub is now empty, move it to the free slubs list.
            if (slub->free_objects == slub->max_objects) {
                list_reinsert_head(&cache->free_slubs, node);
            }
            return;
        }
    }

    list_foreach_safe(&cache->full_slubs, node) {
        struct slub *slub = list_entry(node, struct slub, slub_node);
        if (slub_contains(slub, obj)) {
            // Since the slub is now partially full, move it to the
            // partial slubs list.
            list_reinsert_head(&cache->partial_slubs, node);
            slub_add_to_free_list(slub, obj);
            cache->free_obj_count++;
            slub->free_objects++;
            return;
        }
    }

    if (cache->flags & SLUB_DEBUG) {
        debug("%s cache : cannoy free unknown object 0x%p", cache->name, obj);
    }
}

/**
 * @brief Allocate an object from the given cache.
 * 
 * @param cache The cache from which to allocate the object.
 * @return void* A pointer to the allocated object if successful, or NULL if
 * the allocation failed (likely due to an out-of-memory condition).
 */
void *slub_alloc(struct slub_cache *cache)
{
    struct list_head *pool = &cache->partial_slubs;
    struct slub *slub = NULL;

    // If the partial slubs list is empty, try to use the free slubs list. If
    // the free slubs list is also empty, add a new slub to the cache. If the
    // slub cache could not be allocated, return NULL.
    if (list_empty(&cache->partial_slubs)) {
        if (list_empty(&cache->free_slubs)) {
            if (!slub_add_slub(cache)) {
                warn("Failed to add slub to cache %s", cache->name);
                return NULL;
            }
        }
        pool = &cache->free_slubs;
    }

    if (cache->free_obj_count == cache->min_free) {
        if (!slub_add_slub(cache)) {
            warn("%s %s %s", "Failed to add slub to cache", cache->name,
             "to respect the min_free object count");
            return NULL;
        }
    }

    // Get the first slub from the pool and remove it from the list.
    slub = list_first_entry(pool, struct slub, slub_node);
    cache->free_obj_count--;
    slub->free_objects--;

    if (slub->free_objects == 0) {
        list_reinsert_head(&cache->full_slubs, &slub->slub_node);
    } else if (slub->free_objects == slub->max_objects - 1) {
        list_reinsert_head(&cache->partial_slubs, &slub->slub_node);
    }

    return (void *) list_pop_head(&slub->free_obj_list);
}

/**
 * @brief Destroy the given cache. This function will free all slubs that are
 * allocated from the cache, and then free the cache itself. If the cache is
 * not empty, this function will print a warning message and return without
 * freeing the cache.
 * 
 * @param cache The cache to destroy.
 */
void slub_destroy_cache(struct slub_cache *cache)
{    
    if (!list_empty(&cache->partial_slubs) || 
        !list_empty(&cache->full_slubs)) {
        warn("Cannot destroy cache %s: not empty", cache->name);
        return;
    }

    if (cache->flags & SLUB_DEBUG) {
        debug("Destroying cache %s", cache->name);
    }

    list_foreach(&cache->free_slubs, node) {
        struct slub *slub = list_entry(node, struct slub, slub_node);
        buddy_free_exact((void *) slub->base, 
            align_up(slub->size, PAGE_SIZE) / PAGE_SIZE);
        slub_free(&slub_cache, slub);
    }

    slub_free(&slub_cache_cache, cache);
}

/**
 * @brief Create a new cache for allocating objects of a given size.
 * 
 * @param name The name of the cache (for debugging purposes).
 * @param obj_size The size of the objects to be allocated from the cache.
 * @param obj_align The alignment of the objects in the cache. It must be a
 * power of two. If zero, the default alignment of 8 bytes will be used.
 * @param flags A set of flags that control the behavior of the cache.
 * @return struct slub_cache* The cache created with the given parameters if
 * successful, or NULL if the cache could not be created (likely due to an
 * out-of-memory condition).
 */
struct slub_cache *slub_create_cache(
    const char *name,
    u16 obj_size,
    u16 obj_align,
    u16 min_free,
    uint flags)
{
    struct slub_cache *cache = slub_alloc(&slub_cache_cache);
    if (cache == NULL) {
        return NULL;
    }

    slub_new_cache(cache, name, obj_size, obj_align, min_free, flags);
    return cache;
}
