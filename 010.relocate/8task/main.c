#include "s3c6410.h"
#include "uart.h"
#include "sdram_init.h"
#include "my_printf.h"


int main(void)
{
	uart0_init();
	my_printf_test();

}
