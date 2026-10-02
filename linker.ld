ENTRY(_start)

OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x00010000;

    kernel_start_addr = .;

    .text : ALIGN(16)
    {
        KEEP(*(.text.start))
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(16)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(16)
    {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(16)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    kernel_end_addr = .;

}