SECTIONS
{
   . = 0xB0000000;
   __code_start = .;
   . = ALIGN(4);
   .text	  :
   {
	 start.o
	 clock.o
	 sdram.o
	 nand.o
	 *(.text)
   }
   
   . = ALIGN(4);
   .rodata : { *(.rodata) }
   
    . = ALIGN(4);
   .data : { *(.data) }
   
    . = ALIGN(4);
	__bss_start = .;
   .bss : { *(.bss) *(.COMMON) }
    __bss_end = .;
}

