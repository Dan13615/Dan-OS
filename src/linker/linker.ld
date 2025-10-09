ENTRY(start);
SECTIONS
{
    . = 0x0;
    .boot :
    {
        KEEP(*(.header))
    }

    . = 1M;
    .text :
    {
        *(.text)
    }
}