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
#include <stdarg.h>
#include <lib/str.h>

/**
 * @brief Verify if a string slice is valid. A string slice is valid if the data
 * pointer is not NULL.
 * 
 * @param s The string slice
 * @return true if the slice is valid
 * @return false if the slice is invalid, i.e. the data pointer is NULL
 */
bool str_is_valid(const struct str *s)
{
    return s->data != NULL;
}

/**
 * @brief Verify is a string slice is empty. An invalid slice is considered
 * empty by this function.
 * 
 * @param s The string slice
 * @return true If the slice is empty (len == 0)
 * @return false If the slice contains at least one character
 */
bool str_is_empty(const struct str *s)
{
    return !str_is_valid(s) || s->len == 0;
}


/**
 * @brief Set a character in a string slice at a given index.
 * 
 * @param s The string slice to modify
 * @param data The character to set
 * @param idx The index of the character to set in the string slice
 * @return true If the character was set successfully
 * @return false If the index is out of bounds
 */
bool str_set_char(struct str *s, char data, size_t idx)
{
    if (idx >= s->len) {
        return false;
    }

    s->data[idx] = data;
    return true;
}

/**
 * @brief Return a sub-slice of the given string slice. This function does not
 * allocate memory but simply creates a new struct str with pointers to the
 * same data as the original slice. Therefore, the returned slice is only valid
 * as long as the original slice is valid.
 * 
 * @param s The original string slice
 * @param start The start index of the slice (inclusive)
 * @param end The end index of the slice (exclusive)
 * @return struct str* The resulting slice. If the requested slice is invalid,
 * the function returns a slice with len == 0 and data == NULL.
 */
struct str str_slice(const struct str *s, size_t start, size_t end)
{
    // Check if the request slice is valid:
    // - The start and end indices are within the bounds of the original slice
    // - The start index is less than the end index
    if (start >= s->len || end > s->len || start >= end) {
        return (struct str) {
            .data = NULL,
            .len = 0
        };
    }

    return (struct str) {
        .data = s->data + start,
        .len = end - start
    };
}