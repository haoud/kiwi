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
#include <stddef.h>

void *memmove(void *dst, const void *src, size_t n) {
    const unsigned char *s = (const unsigned char *) src;
    unsigned char *d = (unsigned char *) dst;

    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }

    return dst;
}

void *memcpy(void *restrict dst, const void *restrict src, size_t n) {
    const unsigned char *s = (const unsigned char *) src;
    unsigned char *d = (unsigned char *) dst;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dst;
}

void memset(void *b, int c, size_t len) {
    unsigned char *dst = (unsigned char *) b;
    for (size_t i = 0; i < len; i++) {
        dst[i] = (unsigned char) c;
    }
}