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
#include <arch/gdt.h>
#include <arch/trap.h>
#include <arch/serial.h>
#include <arch/paging.h>
#include <arch/console.h>

/**
 * @brief Initialize the x86 architecture-specific components, such
 * as the GDT, IDT, and serial port.
 */
void arch_x86_setup(void)
{
    console_setup();
    serial_setup();

    gdt_setup();
    trap_setup();
    paging_setup();
}
