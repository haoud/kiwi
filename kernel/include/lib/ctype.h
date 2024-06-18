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
 * @brief Check if a character is a digit
 * 
 * @param c The character to check
 * @return true If the character is a digit
 * @return false If the character is not a digit
 */
static inline bool isdigit(const char c)
{
    return c >= '0' && c <= '9';
}

/**
 * @brief Convert a character to an integer, assuming the character is
 * a digit. If the character is not a digit, the result is undefined.
 * 
 * @param c The character to convert
 * @return unsigned int The integer value of the character
 */
static inline unsigned int ctoi(const char c)
{
    return c - '0';
}