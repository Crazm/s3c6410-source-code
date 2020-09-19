#include "s3c6410.h"
#include "uart.h"

void delay(volatile int t)
{
	while(t--);	
}

int main(void)
{
	unsigned char a;
	uart0_init();
	puts("hello world!\n");
	while(1)
		{
			putchar(getchar());
		}

}