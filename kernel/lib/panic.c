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
#include <lib/panic.h>
#include <lib/string.h>
#include <arch/cpu.h>
#include <arch/serial.h>
#include <arch/console.h>

// Print a message to the serial port and the console. This allows to have
// more debug informations in case of a panic if the serial port and/or the
// console are still working.
#define panic_printf(fmt, ...)              \
    do {                                    \
        serial_printf(fmt, ##__VA_ARGS__);  \
        console_printf(fmt, ##__VA_ARGS__); \
    } while(0)

/**
 * @brief This flags is set to true when the kernel panics. It is used to
 * prevent the kernel from infinitely panicking if an panic occurs during
 * handling a panic. In this case, the kernel will simply halt instead of
 * trying to print debug informations, synchronize buffers...
 */
static bool panicked = false;

/**
 * @brief Write a message to the serial port and halt the system. This
 * function is called when the kernel encounters an unrecoverable error
 * and cannot continue execution.
 * 
 * @param fmt The format string to print
 * @param ... Arguments to the format string
 */
_cold _noreturn
void panic(const char* fmt, ...)
{
    if (!panicked) {
        panicked = true;

        char buffer[PRINTF_BUFFER_SIZE];
        va_list args;

        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        panic_printf("Fatal error: %s\n", buffer);
        panic_printf("Cannot continue, halting...\n");
    }
    
    abort();
}

/**
 * @brief Abort the execution of the program. This function is called when
 * the program encounters an unrecoverable error and cannot continue, not
 * even to print an error message. Prefer using panic() instead of this to
 * allow the kernel to print debug informations in possible.
 */
_cold _noreturn
void abort()
{
    for(;;) {
        cpu_freeze();
    }
}