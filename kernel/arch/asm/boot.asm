# Multiboot 1 header
.set MULTIBOOT_MAGIC, 0x1BADB002
.set MULTIBOOT_FLAGS, 0x00000003
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.set KERNEL_VBASE,  0xC0000000      # Virtual base address of the kernel
.set CR0_PG_BIT,    0x80000000      # Enable paging
.set CR0_WP_BIT,    0x00010000      # Enable write protection for kernel 
.set CR4_PSE_BIT,   0x00000010      # Enable page size extension (4 MiB pages)

.section .multiboot
.align 8
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

.section .init
.extern startup

# The kernel entry point. This is where the kernel starts executing when the
# bootloader jumps to it. It sets up the page directory and page table, and
# enables paging before jumping to the kernel in the upper half of the address
# space. 
.global _start
.align 4
_start:
    cli
    cld

    xorl %edi, %edi
    xorl %ebp, %ebp
    movl $256, %ecx

    # Identity map the first 4 MiB of memory
    movl $(kernel_pd - KERNEL_VBASE), %esi
    movl %edi, (%esi)
    orl $0x83,(%esi)

    # Map the first 768 MiB of memory to the kernel address space. If
    # there is more than 768 MiB of memory, the rest will not be accessible
    # to the kernel. This is a serious limitation, but it is good enough
    # for now.
    addl $768*4, %esi
.L1:
    movl %edi, (%esi)   # Set the page table entry to the physical address
    orl $0x83,(%esi)    # Present, read/write, 4 MiB page

    addl $4, %esi           # Move to the next page table entry
    addl $0x400000, %edi    # Move to the next 4 MiB page
    loop .L1                # Repeat for all 768 MiB of memory

    # Load the physical address of the page directory into CR3
    movl $(kernel_pd - KERNEL_VBASE), %edx
    movl %edx, %cr3

    # Enable page size extension (PSE)
    movl %cr4, %edx
    orl $CR4_PSE_BIT, %edx
    movl %edx, %cr4

    # Enable kernel write protection (WP) and paging (PG)
    movl %cr0, %edx
    orl $(CR0_WP_BIT | CR0_PG_BIT), %edx
    movl %edx, %cr0

    # Jump to the kernel in the upper half
    leal .L2, %ecx
    jmpl *%ecx

.L2:
    movl $stack_top, %esp
    movl $0, %ebp
    pushl %ebx
    jmp startup

.section .bss
.align 4096
.global kernel_pd
kernel_pd:
    .skip 4096

.align 16
stack_bottom:
.skip 4096
stack_top:
