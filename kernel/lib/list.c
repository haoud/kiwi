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
#include <lib/list.h>

/**
 * @brief Insert an element between two elements. Those elements must be
 * adjacent in the list, otherwise the behavior is undefined.
 * 
 * @param prev The previous element of the list.
 * @param next The next element of the list.
 * @param entry The element to insert.
 */
void list_insert(struct list_head *prev,
                 struct list_head *next,
                 struct list_head *entry)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

/**
 * @brief Check if a list is empty.
 * 
 * @param list The list to check.
 * @return true if the list is empty.
 * @return false if the list contains at least one element.
 */
bool list_empty(struct list_head *list)
{
    return (list->next == list);
}

/**
 * @brief Initialize a list entry. The previous and next pointers are set to
 * the entry itself, creating a circular list.
 * 
 * @param list The list to initialize.
 */
void list_init(struct list_head *list)
{
    list->prev = list->next = list;
}

/**
 * @brief Remove an element from a list. The element is not destroyed, only
 * removed from the list.
 * 
 * @param entry the entry to remove.
 */
void list_remove(struct list_head *entry)
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->prev = entry;
    entry->next = entry;
}

/**
 * @brief Add an element at the head of the list.
 * 
 * @param list The list to add the element to.
 * @param entry The element to add.
 */
void list_add_head(struct list_head *list, struct list_head *entry)
{
    list_insert(list, list->next, entry);
}

/**
 * @brief Add an element at the tail of the list.
 * 
 * @param list The list to add the element to.
 * @param entry The element to add.
 */
void list_add_tail(struct list_head *list, struct list_head *entry)
{
    list_insert(list->prev, list, entry);
}

/**
 * @brief Remove an element from the list where it is currently located and
 * insert it at the head of the specified list. The list can be the same as
 * the one where the element is currently located, or a different one.
 * 
 * @param list The list to reinsert the element to.
 * @param entry The element to reinsert.
 */
void list_reinsert_head(struct list_head *list, struct list_head *entry)
{
    list_remove(entry);
    list_add_head(list, entry);
}

/**
 * @brief Remove an element from the list where it is currently located and
 * insert it at the tail of the specified list. The list can be the same as
 * the one where the element is currently located, or a different one.
 * 
 * @param list The list to reinsert the element to.
 * @param entry The element to reinsert.
 */
void list_reinsert_tail(struct list_head *list, struct list_head *entry)
{
    list_remove(entry);
    list_add_tail(list, entry);
}

/**
 * @brief Remove the first element of the list and return it. If the list is
 * empty, return NULL.
 * 
 * @param list The list to pop the head from.
 * @return struct list_head* The first element of the list, or NULL if the list
 * is empty.
 */
struct list_head *list_pop_head(struct list_head *list)
{
    if (list_empty(list)) {
        return NULL;
    }

    struct list_head *entry = list->next;
    list_remove(entry);
    return entry;
}

/**
 * @brief Remove the last element of the list and return it. If the list is
 * empty, return NULL.
 * 
 * @param list The list to pop the tail from.
 * @return struct list_head* The last element of the list, or NULL if the list
 * is empty.
 */
struct list_head *list_pop_tail(struct list_head *list)
{
    if (list_empty(list)) {
        return NULL;
    }

    struct list_head *entry = list->prev;
    list_remove(entry);
    return entry;
}