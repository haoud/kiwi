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

#define IDT_ENTRIES     256

struct idt_descriptor {
    u16 offset0_15;
    u16 selector;
    u8 reserved;
    u8 flags;
    u16 offset16_31;
} __attribute__((packed));

struct idtr {
    u16 size;
    u32 base;
} __attribute__((packed));

void trap_setup(void);

void trap_disable_irq(void);
void trap_enable_irq(void);
