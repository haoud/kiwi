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
#include <multiboot.h>
#include <lib/log.h>
#include <arch/x86.h>
#include <arch/console.h>
#include <mm/page.h>
#include <mm/slub.h>
#include <mm/buddy.h>
#include <mm/malloc.h>

_cdecl _init _noreturn
void startup(struct mb_info *mb_info)
{
    arch_x86_setup(mb_info);
    page_setup(mb_info);
    buddy_setup();
    slub_setup();
    malloc_setup();

    // Test the slub allocator
    struct slub_cache *cache = slub_create_cache("test", 16, 0, 0, SLUB_NONE);
    void *obj1 = slub_alloc(cache);
    void *obj2 = slub_alloc(cache);
    void *obj3 = slub_alloc(cache);

    debug("obj1: %p", obj1);
    debug("obj2: %p", obj2);
    debug("obj3: %p", obj3);

    slub_free(cache, obj1);
    slub_free(cache, obj2);
    slub_free(cache, obj3);
    slub_destroy_cache(cache);

    // Test the malloc() function
    void *ptr1 = malloc(16);
    void *ptr2 = malloc(32);
    void *ptr3 = malloc(56);
    void *ptr4 = malloc(3286);

    debug("ptr1: %p", ptr1);
    debug("ptr2: %p", ptr2);
    debug("ptr3: %p", ptr3);
    debug("ptr4: %p", ptr4);

    free(ptr1);
    free(ptr2);
    free(ptr3);
    free(ptr4);

    info("Boot completed !");
    page_debug_info();
    cpu_freeze(); 
}
