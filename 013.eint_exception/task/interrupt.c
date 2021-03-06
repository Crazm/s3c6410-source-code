
#include "s3c6410.h"
#include "led_on.h"
#include "uart.h"


/* 中断初始化函数 */
void fiq_init(void)
{

	/* 配置GPIO为中断引脚 */
	GPNCON_REG	 &=   ~((3<<0) | (3<<2) | (3<<4) | (3<<6));     //GPNCON[0:7]位清零
	GPNCON_REG	 |=   ((2<<0) | (2<<2) | (2<<4) | (2<<6));      //GPNCON[0:7]=10101010 使能引脚为外部中断
	printHex(GPNCON_REG);
	/*屏蔽上拉下拉电阻*/
	GPNPUD_REG   &=   ~((3<<0) | (3<<2) | (3<<4) | (3<<6));     //GPNPUD[0:7]=00000000 屏蔽上拉下拉电阻
	printHex(GPNPUD_REG);
	/* 设置中断触发方式为: 双边沿触发 */
	EINT0CON0_REG	 &=   ~((7<<0) | (7<<4) );     //EINT0CON0位清零
	EINT0CON0_REG	 |=   ((7<<0) | (7<<4) );      //GPNCON[0:2]=111,[4:6]=111 使能中断触发方式为: 双边沿触发
	printHex(EINT0CON0_REG);

	/* 设置中断模式为fiq，并向向量地址寄存器赋入口参数地址 */
	VIC0INTSELECT_REG  =  0x1;			//设置fiq模式
	printHex(VIC0INTSELECT_REG);
	/* 使能中断屏蔽位 */
	EINT0MASK_REG	&=	 ~((1<<0) | (1<<1)|(1<<2) |(1<<3);	//清除bit0~3，使之可以触发中断
	printHex(EINT0MASK_REG);
	/* 在中断控制器里使能这些中断 */
	VIC0INTENABLE_REG  =  0x1;						//使能中断请求，允许中断到达处理器
	printHex(VIC0INTENABLE_REG);
}

void key_eint_fiq(unsigned int irq)
{
	led_init();

	if (irq == 0) /* eint0 : k1 控制 D1 */
	{
		if (GPNDAT_REG & (1<<0)) 	/* k1 --> gpk4 */
		{
			/* 松开 */
			GPKDAT_REG |= (1<<4);
		}
		else
		{
			/* 按下 */
			GPKDAT_REG &= ~(1<<4);
		}
		
	}

	if (irq == 1) /* eint0 : k2 控制 D2 */
	{
		if (GPNDAT_REG & (1<<1)) 	/* k2 --> gpk5 */
		{
			/* 松开 */
			GPKDAT_REG |= (1<<5);
		}
		else
		{
			/* 按下 */
			GPKDAT_REG &= ~(1<<5);
		}
		
	}

	if (irq == 2) /* eint0 : k3 控制 D3 */
	{
		if (GPNDAT_REG & (1<<2)) 	/* k3 --> gpk6 */
		{
			/* 松开 */
			GPKDAT_REG |= (1<<6);
		}
		else
		{
			/* 按下 */
			GPKDAT_REG &= ~(1<<6);
		}
		
	}

	if (irq == 3) /* eint0 : k4 控制 D4 */
		{
			if (GPNDAT_REG & (1<<3))	/* k4 --> gpk7 */
			{
				/* 松开 */
				GPKDAT_REG |= (1<<7);
			}
			else
			{
				/* 按下 */
				GPKDAT_REG &= ~(1<<7);
			}
			
		}

}

void handle_fiq_c(void)
{
	unsigned int i;
	led_on();
	/* 分辨中断源 并调用对应的处理函数 */
	for(i=0 ; i <4 ; i++)
	{
		if (EINT0PEND_REG & (1<<i) )  
		{
			key_eint_fiq(i); /* 处理中断, 清中断源EINTPEND */	
		}
	}

	/* 清中断 : EINT0~EINT4 */
	EINT0PEND_REG    |= ((1<<0)|(1<<1)|(1<<2)|(1<<3)) ;	//清EINT0PEND相应位,相应位置一
	VIC0ADDRESS_REG  = 0;									//清VIC0ADDRESS
	
}


