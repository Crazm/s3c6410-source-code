#include "s3c6410.h"
#include "led.h"
#include "interrupt.h"
#include "timer.h"
#include "include/stdio.h"
#include "nand.h"
#include "./lcd/lcd.h"
#include "./adc_touchscreen/adc_test.h"
#include "./adc_touchscreen/touchscreen_test.h"
#include "./i2c/i2c_test.h"
#include "./spi/gpio_spi_flash.h"
#include "sensor/DHT11.h"
#include "sensor/DS18B20.h"
#include "sensor/irda_raw.h"


int main(void)
{	
		
	timer0_init();
	
	//led_timer();
	

	lcd_test();

	//adc_test();
	
    //touchscreen_test();

    //i2c_test();

    //spi_flash_test();
    
    //delay_test();

    //dht11_test();
    //ds18b20_test();
    irda_raw_test();

    while(1);
    
	return 0;

}


