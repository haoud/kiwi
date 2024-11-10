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
#include <lib/panic.h>

/**
 * @brief Assert that a condition is true. If the condition is false, the
 * program will panic with an error message. This macro is used to check
 * for runtime conditions and should be used to check for errors in the
 * program.
 */
#define assert(x)                                   \
    do {                                            \
        if(!(x)) {                                  \
            panic("Assertion failed: %s\n", #x);    \
        }                                           \
    } while(0)

/**
 * @brief Assume that a condition is true. If the condition is false, the
 * behaviour is undefined. This macro is used to indicate to the compiler
 * that a condition is always true and may be used by the compiler to
 * optimize the code. However, the condition given to this macro IS NOT
 * evaluated at runtime, but only during the compilation ! You should not
 * rely on this macro to check for runtime conditions, but only to give
 * hints to the compiler.
 */
#define assume(x) __attribute__(assume(x))