SECTIONS {
	.text	0	: { *(.text) }
	.rodata	 	: { *(.rodata) }
	.data 0x50000000 :AT(0x900) { *(.data) }
	.bss : { *(.bss) *(.COMMON) }
}