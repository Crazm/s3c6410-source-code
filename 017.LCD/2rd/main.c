#include "s3c6410.h"
#include "led_on.h"
#include "interrupt.h"
#include "timer_pwm.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "./lcd/lcd.h"

int main(void)
{	
	
	nand_init();

	lcd_test();

    while(1);
	
	return 0;

}


