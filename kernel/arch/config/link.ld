OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    .text : {
        *(.multiboot)
        *(.text)
    }

    .data : {
        *(.data)
    }

    .rodata : {
        *(.rodata)
    }

    .bss (NOLOAD) : {
        *(.bss)
    }

    /DISCARD/ : {
        *(.comment*)
        *(.note*)
    }
}

