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

/// @bried Verify if a number is a power of 2
#define is_power_of_2(x) ({              \
    typeof(x) _x = (x);                  \
    (_x != 0) && ((_x & (_x - 1)) == 0); \
})


/// @brief Align a number to to the nearest greater multiple of another number.
/// The alignment must be a power of 2.
#define align_up(a, b) ({                           \
    typeof(a) _x = (a);                             \
    typeof(b) _al = (b);                            \
    (_x % _al) ? (_x) + (_al - (_x % _al)) : _x;    \
})

/// @brief Align a number to to the nearest smaller multiple of another number.
/// The alignment must be a power of 2.
#define align_down(a, b) ({                         \
    typeof(a) _x = (a);                             \
    typeof(b) _al = (b);                            \
    _x - (_x % _al);                                \
})

/// @bried Verify if a number is aligned to another number. The alignment
/// must be a power of 2.
#define is_aligned(x, a) ({                         \
    typeof(x) _x = (x);                             \
    typeof(a) _al = (a);                            \
    (_x % _al) == 0;                                \
})

/// @brief Get the absolute value of a number
#define abs(x) ({        \
    typeof(x) _x = (x);  \
    (_x < 0) ? -_x : _x; \
})

/// @brief Get the minimum value between two numbers
#define min(a, b) ({        \
    typeof(a) _x = (a);     \
    typeof(b) _y = (b);     \
    (_x < _y) ? _x : _y;    \
})

/// @brief Get the maximum value between two numbers
#define max(a, b) ({        \
    typeof(a) _x = (a);     \
    typeof(b) _y = (b);     \
    (_x > _y) ? _x : _y;    \
})

/// @bried Clamp a number between a minimum and a maximum value
#define clamp(v, min, max) ({                       \
    typeof(v) _v = (v);                             \
    typeof(min) _min = (min);                       \
    typeof(max) _max = (max);                       \
    (_v < _min) ? _min : ((_v > _max) ? _max : _v); \
})

/// @brief Align a number to to the nearest greater multiple of another number.
/// The alignment must be a power of 2.
#define align_up(a, b) ({                           \
    typeof(a) _x = (a);                             \
    typeof(b) _al = (b);                            \
    (_x % _al) ? (_x) + (_al - (_x % _al)) : _x;    \
})
