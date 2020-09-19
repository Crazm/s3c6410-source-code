#include "s3c6410.h"
#include "uart.h"
#include "led_on.h"
#include "sdram.h"


int main(void)
{

	sdram_init();
	if( sdram_test() == 0 )  led_on();

	return 0;

}
