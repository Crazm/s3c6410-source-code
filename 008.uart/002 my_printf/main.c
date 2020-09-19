#include "s3c6410.h"
#include "uart.h"
#include "my_printf.h"

int main(void)
{
	
	uart0_init();
	my_printf_test();
	printf("0x=%d",123456);

}



