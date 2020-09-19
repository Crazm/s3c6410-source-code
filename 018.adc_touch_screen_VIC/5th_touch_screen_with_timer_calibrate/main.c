#include "s3c6410.h"
#include "led_on.h"
#include "interrupt.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "./lcd/lcd.h"
#include "./adc_touchscreen/adc_test.h"
#include "./adc_touchscreen/touchscreen_test.h"


int main(void)
{	
		
	timer0_init();
	
	led_timer();
	

	lcd_test();

	//adc_test();
	
    touchscreen_test();
    
    while(1);
	
	return 0;

}


