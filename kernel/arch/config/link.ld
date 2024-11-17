OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_start)

SECTIONS
{
    . = 0xC0100000;
    .init BLOCK(4K) : AT(ADDR(.init) - 0xC0000000) {
        *(.multiboot)
        *(.init*)
    }

	.text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000) {
        *(.text)
    }

	.rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata)
    }

	.data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000) {
        *(.data)
    }

    .bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000) {
        *(COMMON)
        *(.bss)
    }

    . = ALIGN(4k);
    __end = .;

    /DISCARD/ : {
        *(.comment*)
        *(.note*)
    }
}
