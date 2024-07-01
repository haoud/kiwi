/**
 * Copyright (C) 2022 Romain CADILHAC
 *
 * This file is part of Silicium.
 *
 * Silicium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Silicium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Silicium. If not, see <http://www.gnu.org/licenses/>.
 */
#include <lib/ubsan.h>
#include <arch/serial.h>

/**
 * @brief Print an error message and halt the system.
 * 
 * @param location The location of the error in the source code
 * @param message The error message
 */
_ubsan
static void ubsan_abort(
    const struct ubsan_source_location *location,
    const char *message)
{
    serial_printf("%s:%u:%u ubsan abort : %s\n",
        location->file,
        location->line,
        location->column,
        message
    );

    for(;;) {}
}

_ubsan
void __ubsan_handle_type_mismatch_v1(void *data,
    [[maybe_unused]] void *ptr)
{
    struct ubsan_type_mismatch_info_v1 *info = data;
    unsigned int align = 1 << info->log_alignment;
    uintptr_t pointer = (uintptr_t) ptr;

    if (pointer == 0) {
        ubsan_abort(&info->location, "null pointer");
    } else if (align && (pointer & (align - 1))) {
        ubsan_abort(&info->location, "unaligned pointer");
    } else {
        ubsan_abort(&info->location, "type mismatch");
    }
}

_ubsan
void __ubsan_handle_add_overflow(void *data,
    [[maybe_unused]] void *lhs,
    [[maybe_unused]] void *rhs)
{
    struct ubsan_overflow_desc *desc = data;
    ubsan_abort(&desc->location, "add overflow");
}

_ubsan
void __ubsan_handle_sub_overflow(void* data,
    [[maybe_unused]] void* lhs,
    [[maybe_unused]] void* rhs)
{
    struct ubsan_overflow_desc *desc = data;
    ubsan_abort(&desc->location, "sub overflow");
}

_ubsan
void __ubsan_handle_mul_overflow(void *data, 
    [[maybe_unused]] void *lhs,
    [[maybe_unused]] void *rhs)
{
    struct ubsan_overflow_desc *desc = data;
    ubsan_abort(&desc->location, "mul overflow");
}

_ubsan
void __ubsan_handle_negate_overflow(void *data, 
    [[maybe_unused]] void *old_value)
{
    struct ubsan_overflow_desc *desc = data;
    ubsan_abort(&desc->location, "negate overflow");
}

_ubsan
void __ubsan_handle_shift_out_of_bounds(void *data,
    [[maybe_unused]] void *lhs,
    [[maybe_unused]] void *rhs)
{
    struct ubsan_shift_info *info = data;
    ubsan_abort(&info->location, "shift out of bounds");
}

_ubsan
void __ubsan_handle_out_of_bounds(void* data,
    [[maybe_unused]] void* index)
{
    struct ubsan_out_of_bounds_info *info = data;
    ubsan_abort(&info->location, "out of bounds");
}

_ubsan
void __ubsan_handle_builtin_unreachable(void* data)
{
    struct ubsan_unreachable_info *info = data;
    ubsan_abort(&info->location, "unreachable reached");   
}

_ubsan
void __ubsan_handle_missing_return(void* data)
{
    struct ubsan_unreachable_info *info = data;
    ubsan_abort(&info->location, "missing return");
}

_ubsan
void __ubsan_handle_function_type_mismatch(void *data,
    [[maybe_unused]] void *value)
{
    struct ubsan_type_mismatch_info *info = data;
    ubsan_abort(&info->location, "function type mismatch");
}

_ubsan
void __ubsan_handle_nonnull_return(void *data)
{
    struct ubsan_nonnull_return_info *info = data;
    ubsan_abort(&info->location, "nonnull returned null");
}

_ubsan
void __ubsan_handle_pointer_overflow(void *data, 
    [[maybe_unused]] void *base,
    [[maybe_unused]] void *result)
{
    struct ubsan_ptr_overflow_info *info = data;
    ubsan_abort(&info->location, "pointer overflow");
}

_ubsan
void __ubsan_handle_divrem_overflow(void *data, 
    [[maybe_unused]] void *lhs,
    [[maybe_unused]] void *rhs)
{
    struct ubsan_overflow_desc *info = data;
    ubsan_abort(&info->location, "divrem overflow");
}
