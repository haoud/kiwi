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
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef intptr_t iptr;
typedef uintptr_t uptr;

/// Assume that a condition is unlikely to be true. This macro is used to give
/// hints to the compiler that a condition is unlikely to be true and may be
/// used by the compiler to optimize the code.
#define unlikely(x)     __builtin_expect(!!(x), 0)

/// Assume that a condition is likely to be true. This macro is used to give
/// hints to the compiler that a condition is likely to be true and may be used
/// by the compiler to optimize the code. 
#define likely(x)       __builtin_expect(!!(x), 1)

/// A function attribute to specify that a function does not return. This
/// attribute is used to inform the compiler that a function will never return
/// and allows the compiler to perform optimizations on the function.
///
/// Misusing this attribute can lead to undefined behaviour if the function
/// actually returns, so it should be used with caution.
#define _noreturn       __attribute__((noreturn))

/// A function attribute to specify that a function is cold. Cold functions are
/// functions that are called infrequently. This attribute allows the compiler
/// to optimize the function for size and to place the function in a special
/// section of the binary which packs all cold functions together, to improve
/// the cache locality of the program and to reduce the probability of the
/// instructions being in the cache since the function is not critical for the
/// performance of the program.
#define _cold   __attribute__((cold))

/// A function attribute to specify that a function is hot. Hot functions are
/// functions that are called frequently and any optimization on those functions
/// will have a significant impact on the performance of the program.
///
/// The hot attribute is used to optimize the function for speed and to place
/// the function in a special section of the binary which packs all hot 
/// functions together, to improve the cache locality of the program and
/// increase the probability of the instructions being in the cache.
#define _hot    __attribute__((hot))

/// A function attribute to specify that a function is const. Const functions
/// are functions that do not modify the state of the program and only depend
/// on their arguments to produce a result. This attribute allows the compiler
/// to perform optimizations on the function.
///
/// If pointer arguments are passed to a const function, the function should not
/// dereference the pointer or modify the data it points to.
///
/// Examples of const functions include abs, sqrt, or exp. Those functions will
/// always return the same result for the same input values without modifying
/// any global or static variables.
#define _const  __attribute__((const))

/// A function attribute to specify that a function is pure and allows the
/// compiler to perform optimizations on the function.
/// The pure attribute prohibits a function from modifying the state of the
/// program that is observable by means other than inspecting the function’s
/// return value. However, functions declared with the pure attribute can safely
/// read any non-volatile objects, and modify the value of objects in a way that
/// does not affect their return value or the observable state of the program. 
///
/// Examples of pure functions include strlen or memcmp since they do not modify
/// the state of the program, but dereferencing a pointer forbids those 
/// functions to be marked as const.
#define _pure   __attribute__((pure))

/// A function attribute to specify that a function is used only during the
/// initialization of the kernel. When the kernel is fully initialized, all
/// functions marked with this attribute are removed from the binary to reduce
/// the size of the kernel.
///
/// Calling a function marked with this attribute after the kernel is fully
/// initialized will result in undefined behaviour.
#define _init   __attribute__((section(".init")))

/// A function attribute to specify that a function must use the cdecl calling
/// convention. The cdecl calling convention is the default calling convention
/// used by the C programming language and is used to specify how function
/// arguments are passed to a function and how the return value is passed back
/// to the caller. In the cdecl calling convention, function arguments are
/// pushed onto the stack from right to left and the caller is responsible for
/// cleaning up the stack after the function returns.
#define _cdecl  __attribute__((cdecl))

/// A function-like macro to get the offset of a member in a struct. This macro
/// is used to get the structure address of `type` from the address of `member`
/// in the structure.
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
