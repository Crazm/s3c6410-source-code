#include "s3c6410.h"
#include "uart.h"
#include "led_on.h"
#include "interrupt.h"


int main(void)
{	
	int i=0;
	while(1)
		{
			printHex(i);
			puts("\n\r");
			i++;
		}

	return 0;

}
