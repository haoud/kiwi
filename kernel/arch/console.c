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
#include <memory.h>
#include <arch/console.h>

static const unsigned int HEIGHT = 25;
static const unsigned int WIDTH = 80;
static const unsigned int BASE = 0xB8000;
static unsigned int COLOR = 0x07;
static unsigned int X = 0;
static unsigned int Y = 0;

/**
 * @brief Print a character at the current cursor position. This function
 * handle a few special characters:
 * - '\t' will ident the cursor to the next tabulation stop (every 4 characters)
 * - '\r' will move the cursor to the beginning of the line
 * - '\n' will move the cursor to the beginning of the next line
 * 
 * @param character The character to print at the current cursor position
 */
static void console_putc(const char character) 
{
    char *video_memory = (char *) (BASE + X * 2 + Y * WIDTH * 2);
    switch (character) {
        case '\t':
            for (unsigned int i = 0; i < 4 - (X % 4); i++) {
                video_memory[i * 2] = character;
                video_memory[i * 2 + 1] = COLOR;
            } 
            break;

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
 * @brief Setup the console subsystem. This function must be called before
 * using any other console function. It will use the VGA text mode to display
 * text on the screen, and clear the screen in case of garbage data in the
 * video memory.
 */
void console_setup()
{
    console_clear();
}

/**
 * @brief Verify if the console subsystem is available. This function will
 * always return true, as we assume that the VGA text mode is always available
 * on x86 systems.
 * 
 * @return true 
 * @return false 
 */
bool console_exist()
{
    return true;
}

/**
 * @brief Clear the screen by filling the video memory with spaces and the
 * current color attribute.
 * 
 */
void console_clear()
{
    char *video_memory = (char *) BASE;
    for (unsigned int i = 0; i < HEIGHT * WIDTH; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
}

/**
 * @brief Scroll up the screen by moving the content of the video memory one
 * line up.
 * 
 */
void console_scrollup() 
{
    const unsigned int len = (HEIGHT - 1) * WIDTH * 2;
    void *src = (void *) (BASE + WIDTH * 2);
    void *dst = (void *) BASE;
    memmove(dst, src, len);
}

/**
 * @brief Write a string to the console. This function will print each character
 * of the string using the console_putc function. If the console subsystem is
 * not available, this function will do nothing.
 * 
 * @param str The string to print
 */
void console_write(const char *str)
{
    if (console_exist()) {
        while (*str != '\0') {
            console_putc(*str++);
        }
    }
}