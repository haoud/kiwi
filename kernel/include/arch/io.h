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

/**
 * @brief Write a byte to a port. Be careful when using this function as writing
 * to a port that does not exist can cause undefined behavior, and even writing
 * to a port that does exist can cause issues if the hardware is not expecting
 * the data and can have side effects that must be taken into account.
 * 
 * @param port The port to write to 
 * @param data The byte to write
 */
static inline void out8(u16 port, u8 data) 
{
    asm volatile("outb %0, al" :: "Nd"(port), "a"(data));
}

/**
 * @brief Write a word to a port. Be careful when using this function as writing
 * to a port that does not exist can cause undefined behavior, and even writing
 * to a port that does exist can cause issues if the hardware is not expecting
 * the data and can have side effects that must be taken into account.
 * 
 * @param port The port to write to 
 * @param data The word to write
 */
static inline void out16(u16 port, u16 data) 
{
    asm volatile("outw %0, ax" :: "Nd"(port), "a"(data));
}

/**
 * @brief Write a double word to a port. Be careful when using this function as
 * writing to a port that does not exist can cause undefined behavior, and even
 * writing to a port that does exist can cause issues if the hardware is not
 * expecting the data and can have side effects that must be taken into account.
 * 
 * @param port The port to write to 
 * @param data The double word to write
 */
static inline void out32(u16 port, u32 data) 
{
    asm volatile("outd %0, eax" :: "Nd"(port), "a"(data));
}

/**
 * @brief Read a byte from a port. If the port does not exist, the behavior 
 * is undefined.
 * 
 * @param port The port to read from
 * @return u8 The byte read from the port
 */
static inline u8 in8(u16 port) 
{
    u8 data;
    asm volatile("inb al, %1"
                 : "=a"(data)
                 : "Nd"(port));
    return data;                        
}

/**
 * @brief Read a word from a port. If the port does not exist, the behavior
 * is undefined.
 * 
 * @param port The port to read from
 * @return u16 The word read from the port
 */
static inline u16 in16(u16 port) 
{
    u16 data;
    asm volatile("inw ax, %1"
                 : "=a"(data)
                 : "Nd"(port));
    return data;                        
}

/**
 * @brief Read a double word from a port. If the port does not exist, the
 * behavior is undefined.
 * 
 * @param port The port to read from
 * @return u32 The double word read from the port
 */
static inline u32 in32(u16 port) 
{
    u32 data;
    asm volatile("ind ax, %1"
                 : "=a"(data)
                 : "Nd"(port));
    return data;                        
}

/**
 * @brief Wait for an I/O operation to complete by writing to port 0x80, 
 * assuming that this port is not used by any hardware. This was used by
 * the Linux kernel so it must be relatively safe to use.
 */
static inline void iowait(void)
{
    out8(0x80, 0);
}