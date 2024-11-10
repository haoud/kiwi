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

struct idt_handler {
    u8 opcode[16];
} __attribute__((packed));

extern struct idt_handler interrupt_handlers[IDT_ENTRIES];

static struct idt_descriptor IDT[IDT_ENTRIES] = {};
static struct idtr IDTR = {
    .size = sizeof(IDT) - 1,
    .base = (u32) IDT
};

/**
 * @brief Set an interrupt gate in the IDT. Interrupt gates are used for
 * hardware interrupts and cannot be directly called by the user. When an
 * interrupt gate is called, interrupts are disabled to prevent reentrancy.
 * 
 * @param offset The index in the IDT. This must be less than IDT_ENTRIES.
 * @param handler The handler function to call when the interrupt is triggered.
 */
static void idt_set_interrupt_gate(int offset, void *handler)
{
    IDT[offset].offset16_31 = ((u32) handler >> 16) & 0xFFFF;
    IDT[offset].offset0_15 = (u32) handler & 0xFFFF;
    IDT[offset].selector = GDT_KERNEL_CS;
    IDT[offset].reserved = 0;
    IDT[offset].flags = 0x8E;
}

/**
 * @brief Set a trap gate in the IDT. Trap gates are used for software
 * interrupts and cannot be directly called by the user. Contrary to an
 * interrupt gate, a trap gate does not disable interrupts when called
 * and thus can be called recursively. This should correctly be handled
 * by the handler.
 * 
 * @param offset The index in the IDT. This must be less than IDT_ENTRIES.
 * @param handler The handler function to call when the interrupt is triggered.
 */
static void idt_set_trap_gate(int offset, void *handler)
{
    IDT[offset].offset16_31 = ((u32) handler >> 16) & 0xFFFF;
    IDT[offset].offset0_15 = (u32) handler & 0xFFFF;
    IDT[offset].selector = GDT_KERNEL_CS;
    IDT[offset].reserved = 0;
    IDT[offset].flags = 0x8F;
}

/**
 * @brief Set a system gate in the IDT. System gates are used for system
 * calls and other system related traps and can be directly called by the
 * user. When a system gate is called, the CPU switches to the kernel stack
 * and the kernel code segment and disables interrupts to prevent reentrancy.
 * 
 * @param offset The index in the IDT. This must be less than IDT_ENTRIES.
 * @param handler The handler function to call when the interrupt is triggered.
 */
static void idt_set_system_gate(int offset, void *handler)
{
    IDT[offset].offset16_31 = ((u32) handler >> 16) & 0xFFFF;
    IDT[offset].offset0_15 = (u32) handler & 0xFFFF;
    IDT[offset].selector = GDT_KERNEL_CS;
    IDT[offset].reserved = 0;
    IDT[offset].flags = 0xEE;
}

/**
 * @brief Setup the trap table by setting the IDT to handle exceptions and
 * IRQs. Other traps are not handled by the kernel and are left to an default
 * handler that does nothing.
 */
_init
void trap_setup(void)
{
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_interrupt_gate(i, &interrupt_handlers[i]);
    }

    asm volatile("lidt %0" : : "m" (IDTR) : "memory");
}

/**
 * @brief Disable interrupts on the current CPU core by clearing the IF flag
 * in the EFLAGS register. However, this does not prevent the CPU from
 * throwing exceptions.
 */
void trap_disable_irq(void)
{
    asm volatile("cli" : : : "memory");
}

/**
 * @brief Enable interrupts on the current CPU core by setting the IF flag
 * in the EFLAGS register.
 */
void trap_enable_irq(void)
{
    asm volatile("sti" : : : "memory");
}