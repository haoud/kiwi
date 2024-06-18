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
#include <stdarg.h>
#include <lib/str.h>

// Display a minus sign if the number is negative
#define FORMAT_MINUS    0x01

// Display a plus sign if the number is positive
#define FORMAT_PLUS     0x02

// Pad the number with spaces instead of zeros
#define FORMAT_SPACE    0x04

// Pad the number with zeros instead of spaces
#define FORMAT_ZERO     0x08

// Display the number in hexadecimal with a leading "0x"
#define FORMAT_HASH     0x10

// Display the number in uppercase hexadecimal
#define FORMAT_UPPER    0x20

// Display the number as a signed integer
#define FORMAT_SIGNED   0x40


/// Formatting options for the number() function
struct number_format {
    /// Number of padding characters to add before the number, including the sign 
    /// if requested by the FORMAT_PLUS or FORMAT_MINUS flags.
    unsigned int padding;

    /// Formatting flags (see FORMAT_* constants)
    unsigned int flags;
    
    /// Base to use for the number, between 2 and 16.
    unsigned int base;
};


struct str number(struct str buffer, int number, struct number_format format);
int vvsnprintfsnprintf(char *buffer, size_t n, const char *format, va_list arg);
int snprintf(char *buffer, size_t n, const char *format, ...);
size_t strlen(const char *s);
