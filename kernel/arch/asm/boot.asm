.intel_syntax noprefix

# Multiboot 1 header
.set MULTIBOOT_MAGIC, 0x1BADB002
.set MULTIBOOT_FLAGS, 0x00000003
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.section .multiboot
.align 8
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

.section .text
.extern startup
.global _start
.align 4
_start:
  mov esp, stack_top
  call startup

.section .bss
stack_bottom:	
.space 4096
stack_top:
