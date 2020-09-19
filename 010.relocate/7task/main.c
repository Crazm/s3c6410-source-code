#include "s3c6410.h"
#include "uart.h"
#include "sdram_init.h"


int main(void)
{
	extern int __code_start,__bss_start;
	unsigned int *p= &__code_start;
	unsigned int *q= &__bss_start;
	uart0_init();
	puts("\n\rcode_start=");
	printfHEX(&__code_start);
	puts("\tbss_start=");
	printfHEX(&__bss_start);
	puts("\n\r[code_start]=");
	while(p<=q)
		{	
			printfHEX(*p++);
			putchar(' ');
		}
	return 0;

}
