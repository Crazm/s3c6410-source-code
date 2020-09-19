#include "s3c6410.h"
#include "uart.h"
#include "led_on.h"

void led_ctrl(unsigned int);


int main(void)
{
	
	while(1)
		{
			led_ctrl(0);
			delay(1000000);
			led_ctrl(1);
			delay(1000000);
		}

	return 0;

}
