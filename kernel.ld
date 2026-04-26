ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        /* ALIGN(8) before __bss_start ensures the BSS zeroing loop in _start
         * begins at an 8-byte aligned address. The loop uses `sd` (store
         * doubleword, 8 bytes at a time), which traps on RISC-V if the address
         * is not 8-byte aligned. ALIGN(8) after the contents ensures __bss_end
         * is also aligned, so the total BSS size is a multiple of 8 and the
         * loop always terminates on an aligned boundary. */
        . = ALIGN(8);
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }

    . = ALIGN(16);
    . = . + 4096;
    _stack_top = .;
}
