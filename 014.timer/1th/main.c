#include "s3c6410.h"
#include "uart.h"
#include "led_on.h"
#include "interrupt.h"


int main(void)
{	
	int i=0;
	timer_init();
	led_init();
	while(1)
		{
			printHex(i);
			puts("\n\r");
			i++;
		}

	return 0;

}
