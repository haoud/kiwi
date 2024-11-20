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

/// @brief A intrusive double linked list entry.
struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

/// @brief Declare an empty double linked list.
#define DECLARE_LIST(name)    \
    struct list_head name = { \
        &name, &name          \
    }

/// @brief Get the first element of the list if called of the list structure. If
/// called on an element of the list, the behavior is undefined.
#define list_first(list) \
    ((list)->next)

/// @brief Get the last element of the list if called of the list structure. If
/// called on an element of the list, the behavior is undefined.
#define list_last(list) \
    ((list)->prev)

/// @brief Get the container structure of an element in a list.
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/// @brief Get the container structure of the previous element in a list.
#define list_prev_entry(ptr, type, member)  \
    container_of((ptr)->prev, type, member)

/// @brief Get the container structure of the next element in a list.
#define list_next_entry(ptr, type, member)  \
    container_of((ptr)->next, type, member)

/// @brief Iterate over a list. In the loop body, an variable `entry` is defined
/// that represents the current element of the list.
#define list_foreach(list, entry)                \
    for (struct list_head *entry = (list)->next; \
         (entry) != (list);                      \
         (entry) = (entry)->next)

/// @brief Iterate over a list using an already defined variable `entry` that
/// represents the current element of the list. 
#define list_foreach_d(list, entry) \
    for ((entry) = (list)->next;    \
         (entry) != (list);         \
         (entry) = (entry)->next)

/// @brief Iterate over a list in reverse order. In the loop body, an variable
/// `entry` is defined that represents the current element of the list.
#define list_foreach_r(list, entry)              \
    for (struct list_head *entry = (list)->prev; \
         (entry) != (list);                      \
         (entry) = (entry)->prev)

/// @brief Iterate over a list in reverse order using an already defined 
/// variable `entry` that represents the current element of the list.
#define list_foreach_r_d(list, entry) \
    for ((entry) = (list)->prev;      \
         (entry) != (list);           \
         (entry) = (entry)->prev)

/// @brief Iterate over a list safely. In the loop body, an variable `entry` is
/// defined that represents the current element of the list. This variant of list
/// iteration allow removing the current element from the list without affecting
/// the iteration since the next element is saved in a temporary variable.
#define list_foreach_safe(list, entry)             \
    for (struct list_head *entry = (list)->next,   \
                          *__next = (entry)->next; \
         (entry) != (list);                        \
         (entry) = __next, __next = (entry)->next)

/// @brief Iterate over a list safely using an already defined variable `entry`
/// that represents the current element of the list. This variant of list
/// iteration allow removing the current element from the list without affecting
/// the iteration since the next element is saved in a temporary variable.
#define list_foreach_safe_d(list, entry)           \
    entry = (list)->next;                          \
    for (struct list_head *__next = (entry)->next; \
         (entry) != (list);                        \
         (entry) = __next, __next = (entry)->next)

/// @brief Iterate over a list in reverse order safely. In the loop body, an
/// variable `entry` is defined that represents the current element of the
/// list. This variant of list iteration allow removing the current element from
/// the list without affecting the iteration since the previous element is saved
/// in a temporary variable.
#define list_foreach_safe_r(list, entry, tmp)       \
    for (struct list_head *entry = (list)->prev,    \
                          *__prev = (entry)->prev;  \
         (entry) != (list);                         \
         (entry) = __prev, __prev = (entry)->prev)

/// @brief Iterate over a list in reverse order safely using an already defined
/// variable `entry` that represents the current element of the list. This 
/// variant of list iteration allow removing the current element from the list
/// without affecting the iteration since the previous element is saved in a
/// temporary variable.
#define list_foreach_safe_r_d(list, entry)         \
    entry = (list)->prev;                          \
    for (struct list_head *__prev = (entry)->prev; \
         (entry) != (list);                        \
         (entry) = __prev, __prev = (entry)->prev)

void list_insert(struct list_head *prev,
                 struct list_head *next,
                 struct list_head *entry);
bool list_empty(struct list_head *list);
void list_init(struct list_head *list);
void list_remove(struct list_head *entry);
void list_add_head(struct list_head *list, struct list_head *entry);
void list_add_tail(struct list_head *list, struct list_head *entry);
void list_reinsert_head(struct list_head *list, struct list_head *entry);
void list_reinsert_tail(struct list_head *list, struct list_head *entry);
struct list_head *list_pop_head(struct list_head *list);
struct list_head *list_pop_tail(struct list_head *list);