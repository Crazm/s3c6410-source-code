#include "s3c6410.h"
#include "uart.h"
#include "led_on.h"

char g_char1 ='A'; 	//全局变量
char g_char2 ='a'; 	//全局变量
char g_char3 = 0;
char g_char4 ;

int main(void)
{
	uart0_init();
	puts("\n\rg_char3=");
	printfHEX(g_char2);
	puts("\n\r");
	while(1)
		{
			putchar(g_char1);
			g_char1++;
			putchar(g_char2);
			g_char2++;
			delay(100000);
		}

	return 0;

}
