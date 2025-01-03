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
#include <memory.h>
#include <arch/io.h>
#include <arch/paging.h>
#include <arch/console.h>
#include <lib/string.h>

static const unsigned int BASE = KERNEL_VBASE + 0xB8000;
static const unsigned int HEIGHT = 25;
static const unsigned int WIDTH = 80;

static unsigned int COLOR = 0x07;
static unsigned int X = 0;
static unsigned int Y = 0;

/**
 * @brief Print a character at the current cursor position. This function
 * handle a few special characters:
 * - '\r' will move the cursor to the beginning of the line
 * - '\n' will move the cursor to the beginning of the next line
 * 
 * @param character The character to print at the current cursor position
 */
static void console_putc(const char character) 
{
    char *video_memory = (char *) (BASE + X * 2 + Y * WIDTH * 2);
    switch (character) {
        case '\n':
            Y += 1;
            [[fallthrough]];

        case '\r':
            X = 0;
            break; 

        default:
            video_memory[0] = character;
            video_memory[1] = COLOR;
            X += 1;
            break;
    }

    // If we reach the end of the line, we move to the next line
    if (X >= WIDTH) {
        Y += 1;
        X = 0;
    }

    // If we reach the end of the screen, we scroll up
    // FIXME: Handle scrolling multiple lines
    if (Y >= HEIGHT) {
        console_scrollup();
    }
}

/**
 * @brief Enable or disable the hardware cursor. The cursor is a blinking
 * underscore that indicates the current position of the text input. If the
 * cursor is disabled, it will not be displayed on the screen. This may be 
 * useful if you want an software cursor instead that can be more flexible.
 * 
 * @param enable A boolean to enable or disable the cursor
 */
static void console_enable_cursor(bool enable) 
{
    if (enable) {
        out8(0x3D4, 0x0A);
        out8(0x3D5, in8(0x3D5) & ~0x10);
    } else {
        out8(0x3D4, 0x0A);
        out8(0x3D5, in8(0x3D5) | 0x10);
    }
}

/**
 * @brief Set the cursor position on the screen. The cursor position is
 * defined by the x and y coordinates. The origin (0, 0) is the top-left 
 * corner of the screen.
 * If the coordinates are out of range, this function will do nothing.
 * 
 * @param x The x coordinate of the cursor
 * @param y The y coordinate of the cursor
 */
static void console_set_cursor(unsigned int x, unsigned int y)
{
    // TODO: Verify that x and y are not out of range
    unsigned int offset = x + y * WIDTH;
    
    out8(0x3D4, 0x0F);
    out8(0x3D5, offset & 0xFF);
    out8(0x3D4, 0x0E);
    out8(0x3D5, (offset >> 8) & 0xFF);
}

/**
 * @brief Setup the console subsystem. This function must be called before
 * using any other console function. It will use the VGA text mode to display
 * text on the screen, and clear the screen in case of garbage data in the
 * video memory.
 */
_init
void console_setup()
{
    console_clear();
    console_enable_cursor(true);
    console_set_cursor(0, 0);
}

/**
 * @brief Verify if the console subsystem is available. This function will
 * always return true, as we assume that the VGA text mode is always available
 * on x86 systems.
 * 
 * @return true if the console subsystem is available
 * @return false if the console subsystem is not available
 */
bool console_exist()
{
    return true;
}

/**
 * @brief Clear the screen by filling the video memory with spaces and the
 * current color attribute.
 */
void console_clear()
{
    char *video_memory = (char *) BASE;
    for (unsigned int i = 0; i < HEIGHT * WIDTH; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = COLOR;
    }
}

/**
 * @brief Scroll up the screen by moving the content of the video memory one
 * line up.
 */
void console_scrollup() 
{
    const unsigned int len = (HEIGHT - 1) * WIDTH * 2;
    void *src = (void *) (BASE + WIDTH * 2);
    void *dst = (void *) BASE;
    memmove(dst, src, len);
}

/**
 * @brief Print a string on the screen. This function will print the string
 * character by character, and handle special characters like '\n' and '\r'.
 * After printing the string, the hardware cursor will be moved to the current
 * position if enabled. If the console subsystem is not available, this function
 * will do nothing.
 * 
 * @param str The string to print
 */
void console_write(const char *str)
{
    if (console_exist()) {
        while (*str != '\0') {
            console_putc(*str++);
        }
        console_set_cursor(X, Y);
    }
}

/**
 * @brief Write a formatted string to the console. This function is similar
 * to printf, but it only supports a subset of the formatting options. The
 * formatted string will be written to the console using console_write.
 * 
 * @param format  The format string
 * @param ... Optional arguments to format depending on the format string
 */
void console_printf(const char *format, ...)
{
    char buffer[PRINTF_BUFFER_SIZE];
    va_list ap;
    
    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);
    
    console_write(buffer);
}