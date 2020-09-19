#include "s3c6410.h"
#include "uart.h"
#include "led_on.h"
#include "interrupt.h"
#include "timer_pwm.h"

int raise(void)
{
	return 0;
}

int main(void)
{	
	int i;
	int j=50;
	timer0_pwm_init();
	while(1)
		{		
		 i=500000;
	     timer0_pwm(j,60);
		 j += 50;
		 while(i--);
		}

	return 0;

}
