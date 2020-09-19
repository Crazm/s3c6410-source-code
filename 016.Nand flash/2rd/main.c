#include "s3c6410.h"
#include "led_on.h"
#include "interrupt.h"
#include "timer_pwm.h"
#include "my_printf.h"
#include "nand_flash.h"

int main(void)
{	
	
	nand_init();

	nand_flash_test();
	
	return 0;

}
