.intel_syntax noprefix

# A macro to generate interrupt handlers for all interrupts. Depending on the
# interrupt number, the handler will push different values to the stack to 
# uniformize the interrupt handling between the different interrupts. This
# allows us to have a single interrupt handler for all interrupts and greatly
# decrease boilerplate code.
.altmacro
.macro generate_handler irq
  .if irq == 8 || irq == 10 || irq == 11 || irq == 12 || irq == 13
    .align 16
    .global irq_\irq
    irq_\irq :
      push 0
      push irq
      push 0
      jmp interrupt_common
  .elseif  irq == 14 || irq == 17 || irq == 21 || irq == 29 || irq == 30
    .align 16
    .global irq_\irq
    irq_\irq :
      push 0
      push irq
      push 0
      jmp interrupt_common
  .elseif irq < 32
    .align 16
    .global irq_\irq
    irq_\irq :
      push 0
      push 0
      push irq
      push 0
      jmp interrupt_common
  .else
    .align 16
    .global irq_\irq
    irq_\irq :
      push 0
      push 1
      push irq
      push 0
      jmp interrupt_common
  .endif
.endm

# Generate an interrupt handler for each interrupt. The generated array will
# be stored on a single 4 KiB page in the hot section of the kernel, to increase
# the chances of the page being in the cache when an interrupt occurs.
.global interrupt_handlers
.section .text.hot
.align 16
interrupt_handlers:
.set irq, 0
.rept 256
    generate_handler %irq
    .set irq, irq + 1
.endr

interrupt_common:
    add esp, 16
    iret
