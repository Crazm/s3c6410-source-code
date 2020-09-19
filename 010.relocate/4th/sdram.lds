SECTIONS{
   .text   0  : { *(.text) }
   .rodata  : { *(.rodata) }
   .data 0x50000000 : AT(0xb00) 
   { 
	  data_load_addr = LOADADDR(.data);
	  data_start = . ;
	  *(.data) 
	  data_end = . ;
   }
   . = ALIGN(4);
   bss_start = .;
   .bss  : { *(.bss) *(.COMMON) }
   bss_end = .;
}

