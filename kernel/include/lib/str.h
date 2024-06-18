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

/**
 * @brief A string slice.
 */
struct str {
    char *data;
    size_t len;
};

bool str_is_valid(const struct str *s);
bool str_is_empty(const struct str *s);
bool str_set_char(struct str *s, char data, size_t idx);
struct str str_slice(const struct str *s, size_t start, size_t end);
