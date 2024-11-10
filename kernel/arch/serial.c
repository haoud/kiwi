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
#include <config.h>
#include <stdarg.h>
#include <arch/io.h>
#include <arch/serial.h>
#include <lib/string.h>

_init
void serial_setup(void)
{
    // HACK: The kernel does not initialize the serial port because it is
    // only used for debugging purposes under QEMU. This would not work
    // on real hardware, but it is fine for the purposes of this project
    // at the moment.
}

/**
 * @brief Block until the serial port is ready to send a byte, then send it.
 * This function is very simple and incomplete and should not be used in
 * production except for debugging purposes.
 * 
 * @param c The byte to send
 */
void serial_send_byte(const char c)
{
    while ((in8(SERIAL_COM1 + 5) & 0x20) == 0) {
        // TODO: Spinloop hint for the CPU
    }
    out8(SERIAL_COM1, c);
}

/**
 * @brief Send a null-terminated string over the serial port. It will block
 * until the entire string is sent over the serial port.
 * 
 * @param str The string to send
 */
void serial_send_str(const char *str) 
{
    while (*str != '\0') {
        serial_send_byte(*str++);
    }
}

/**
 * @brief Format a string and send it over the serial port. This function is
 * similar to printf, but it only supports a subset of the formatting options.
 * 
 * @param fmt The format string
 * @param ... Optional arguments to format depending on the format string
 */
void serial_printf(const char *fmt, ...)
{
    char buffer[PRINTF_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    serial_send_str(buffer);
}