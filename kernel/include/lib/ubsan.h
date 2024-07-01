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

#define _ubsan __attribute((used))

struct ubsan_source_location {
    const char *file;
    u32 line;
    u32 column;
};
 
struct ubsan_type_descriptor {
    u16 kind;
    u16 info;
    char name[];
};
 
struct ubsan_type_mismatch_info {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
    uintptr_t alignment;
    u8 type_check_kind;
};

struct ubsan_type_mismatch_info_v1 {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
    u8 log_alignment;
    u8 type_check_kind;
};

struct ubsan_shift_info {
    struct ubsan_source_location location;
	struct ubsan_type_descriptor *lhs;
	struct ubsan_type_descriptor *rhs;
};

struct ubsan_overflow_desc {
    struct ubsan_source_location location;
	struct ubsan_type_descriptor *type;
};
 
struct ubsan_out_of_bounds_info {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor left_type;
    struct ubsan_type_descriptor right_type;
};

struct ubsan_nonnull_return_info
{
	struct ubsan_source_location location;
	struct ubsan_source_location attr_location;
};

struct ubsan_ptr_overflow_info {
    struct ubsan_source_location location;
};

struct ubsan_unreachable_info {
    struct ubsan_source_location location;
};