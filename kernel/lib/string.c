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
#include <lib/ctype.h>
#include <lib/string.h>

static const char upper_digits[] = "0123456789ABCDEF";
static const char lower_digits[] = "0123456789abcdef";

/**
 * @brief Format a number and write it to a buffer. The number is written in the
 * specified base and with the specified padding and flags. If the buffer is too
 * small to fully write the number, the function writes as much as possible and
 * returns an empty string slice.
 * 
 * @param buffer The buffer to write to
 * @param number The number to write
 * @param format Formatting options (see struct number_format)
 * @return struct str The remaning buffer after writing the number to it.
 */
struct str number(struct str buffer, int number, struct number_format format) 
{
    const char * digits = lower_digits;
    char padding = ' ';
    char sign = ' ';
    size_t len = 0;

    if (format.flags & FORMAT_MINUS) {
        sign = '-';
    }
    if (format.flags & FORMAT_PLUS) {
        sign = '+';
    }
    if (format.flags & FORMAT_ZERO) {
        padding = '0';
    }
    if (format.flags & FORMAT_UPPER) {
        digits = upper_digits;
    }
    if (format.flags & FORMAT_SIGNED && number < 0) {
        number = -number;
        sign = '-';
    }

    // Convert the number to the specified ASCII base
    char tmp[36];
    do {
        tmp[len++] = digits[(unsigned int) number % format.base];
        number /= format.base;
    } while (number != 0);

    size_t offset = 0;
    size_t i = len;

    // Write the sign to the buffer if necessary
    if (sign != ' ') {
        str_set_char(&buffer, sign, offset);
        offset++, i++;
    }

    // Pad the number with the specified character
    while (i < format.padding) {
        str_set_char(&buffer, padding, offset);
        offset++, i++;
    }

    // Write the number to the buffer
    i = len;
    while (i > 0) {
        str_set_char(&buffer, tmp[--i], offset);
        offset++;
    }

    // Return the remaining buffer
    return str_slice(&buffer, offset, buffer.len);
}

/**
 * @brief Format a string and write it to a buffer. This function only supports
 * a subset of the format specifiers supported by the standard printf function.
 * It does not support floating point numbers, width or precision specifiers,
 * or any other advanced features. If an unknown format specifier is encountered,
 * the function writes it as is to the buffer.
 * 
 * @param buffer The buffer to write to
 * @param size The size of the buffer
 * @param format The format string which can contain format specifiers
 * @param ap The va_list containing the arguments to format
 * @return int The number of characters written to the buffer
 */
int vsnprintf(char *buffer, size_t size, const char *format, va_list ap)
{
    char *start = buffer;
    size_t n = size - 1;

    for (;*format != '\0' && n > 0; format++) {
        // Write the character to the buffer if it is not a format
        // specifier (i.e. not starting with '%')
        if (*format != '%') {
            *buffer = *format;
            buffer++, n--;
            continue;
        }

        unsigned int flags = 0;
        unsigned int base = 10;
        unsigned int pad = 0;
        bool done = false;

        // Parse the formatting flags
        while (!done) {
            switch (*(++format)) {
                case ' ': flags |= FORMAT_SPACE; break;
                case '-': flags |= FORMAT_MINUS; break;
                case '+': flags |= FORMAT_PLUS; break;
                case '0': flags |= FORMAT_ZERO; break;
                case '#': flags |= FORMAT_HASH; break;
                default: done = true; break;
            }
        }

        // Parse the padding count (if any)
        while (isdigit(*format)) {
            pad = pad * 10 + ctoi(*format);
            format++;
        }

        struct number_format fmt = {
            .padding = pad,
            .flags = flags,
            .base = base
        };

        struct str string = {
            .data = buffer,
            .len = n
        };

        switch (*format) {
            // Signed decimal integer
            case 'i':
            case 'd':
                fmt.flags |= FORMAT_SIGNED;
                [[fallthrough]];

            // Unsigned decimal integer
            case 'u':
                string = number(string, va_arg(ap, int), fmt);
                break;

            // Hexadecimal integer with uppercase letters
            case 'X':
                fmt.flags |= FORMAT_UPPER;
                [[fallthrough]];

            // Hexadecimal integer
            case 'x':
                fmt.base = 16;
                string = number(string, va_arg(ap, int), fmt);
                break;

            // Pointer
            case 'p':
                fmt.flags = FORMAT_ZERO;
                fmt.padding = 8;
                fmt.base = 16;

                string = number(string, va_arg(ap, int), fmt);
                break;

            // C string
            case 's':
                const char *str = va_arg(ap, const char *);
                while (*str != '\0' && string.len > 0) {
                    *string.data++ = *str++;
                    string.len--;
                }
                break;

            // Character
            case 'c':
                *string.data++ = va_arg(ap, int);
                string.len--;
                break;
                
            // Unknown format specifier, write it as is to the buffer
            default:
                *string.data++ = *format;
                string.len--;
                break;
        }

        buffer = string.data;
        n = string.len;
    }

    // Write the null terminator to the buffer and return the
    // number of characters written
    start[size - n - 1] = '\0';
    return size - n;
}

/**
 * @brief Format a string and write it to a buffer. This function is a wrapper
 * around the vsnprintf function, allowing to use a dynamic number and type of
 * arguments. It only supports a subset of the format specifiers supported by 
 * the standard printf function. It does not support floating point numbers,
 * width or precision specifiers, or any other advanced features. If an unknown
 * format specifier is encountered, the function writes it as is to the buffer.
 * 
 * @param buffer The buffer to write to
 * @param size The size of the buffer
 * @param format The format string
 * @param ... Variable arguments to format
 * @return int The number of characters written to the buffer
 */
int snprintf(char *buffer, size_t size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buffer, size, format, ap);
    va_end(ap);
    return result;
}

/**
 * @brief Compute the length of a string. This function is O(n) where n is the
 * length of the string, and this is recommended to use it as little as possible
 * in performance-critical code by caching the length of the string when possible.
 * 
 * @param s The string to compute the length of
 * @return size_t The length of the string
 */
size_t strlen(const char *s)
{
    size_t len = 0;
    while (*s++ != '\0') {
        len++;
    }
    return len;
}