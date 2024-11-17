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
#include <stdarg.h>
#include <lib/log.h>
#include <lib/string.h>
#include <lib/assert.h>
#include <arch/serial.h>

const char *level_icon[] = {
    "[ ? ]",    // Undefined
    "[ ~ ]",    // Trace
    "[ # ]",    // Debug
    "[ * ]",    // Info
    "[ - ]",    // Warning
    "[ ! ]",    // Error
    "[!!!]",    // Fatal
};

const char *level_icon_colored[] = {
    "[ ? ]",
    "\033[1m[ ~ ]\033[0m",
    "\033[1m\033[34m[ # ]\033[0m",
    "\033[1m\033[32m[ * ]\033[0m",
    "\033[1m\033[33m[ - ]\033[0m",
    "\033[1m\033[31m[ ! ]\033[0m",
    "\033[1m\033[31m[!!!]\033[0m",
};

void loglevel(int gravity, const char *fmt, ...)
{
    assert(gravity >= LOG_LEVEL_UNDEFINED);
    assert(gravity <= LOG_LEVEL_FATAL);

    char str[LOG_MAX_LEN];

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(str, LOG_MAX_LEN, fmt, arg);
    va_end(arg);

    serial_send_str(str);
}