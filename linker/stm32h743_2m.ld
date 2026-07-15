ENTRY(Reset_Handler)

_estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM);

MEMORY
{
    FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
    DTCMRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
    RAM     (xrw) : ORIGIN = 0x24000000, LENGTH = 512K
    RAM_D2  (xrw) : ORIGIN = 0x30000000, LENGTH = 288K
}

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);

        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;
    } > RAM

    .dma_buffer (NOLOAD) :
    {
        . = ALIGN(32);

        _sdma_buffer = .;

        *(.dma_buffer)
        *(.dma_buffer*)

        . = ALIGN(32);
        _edma_buffer = .;
    } > RAM_D2

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}