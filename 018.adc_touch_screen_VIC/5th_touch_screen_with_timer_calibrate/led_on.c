#include "s3c6410.h"
#include "led_on.h"
#include "timer.h"
#include "my_printf.h"

void led_init(void)
{
	
	GPKCON0_REG &=~((15<<16)|(15<<20)|(15<<24)|(15<<28));//GPK4,5,6,7寄存器清零
	GPKCON0_REG |= ((1<<16)|(1<<20)|(1<<24)|(1<<28));//配置GPK4,5,6,7为输出引脚
	GPKDAT_REG	|= ((1<<4)|(1<<5)|(1<<6)|(1<<7));	//默认灯灭
	
}

void led_on(void)
{
	static unsigned char i=0,j=0,count=0;
	count++;
	if(count == 20)
	{
        j =~i;
    	j &= 15;
    	GPKDAT_REG &= ~(15<<4);//GPKDAT的5,6,7,8位清零
    	GPKDAT_REG |=  (j<<4);
    	i++;
    	if(i>15) i=0;
    	count = 0;
    }	
}

void led_timer(void)
{
    led_init();
    register_timer("led", led_on);
}


void led1_on(void)
{
	GPKDAT_REG   |=  0xe0;
	
}

void led2_on(void)
{
	GPKDAT_REG   |=  0xd0;
	
}

void led3_on(void)
{
	GPKDAT_REG   |=  0xb0;
}

void led4_on(void)
{
	GPKDAT_REG   |=  0x70;
}

/* 延时 */	
void delay(volatile unsigned int t)
{
	while(t--);
}



