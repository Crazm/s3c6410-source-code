
#include "s3c6410.h"
#include "led_on.h"
#include "uart.h"
#include "timer.h"

typedef void(*irq_func)(int);
irq_func irq_array[64];

void register_irq(int irq, irq_func fp)
{
	irq_array[irq] = fp;

	VIC0INTENABLE_REG  |= (1<<irq);		//使能中断请求，允许中断到达处理器
}

/* 中断初始化函数 */
void irq_init(void)
{

	/* 设置中断模式为irq，并向向量地址寄存器赋入口参数地址 */
	VIC0INTSELECT_REG  =  0x0;			//设置irq模式

	/* 在注册中断并使能这些中断 */
	register_irq(23, timer_irq);
	
}

void handle_irq_c(void)
{
	unsigned int i,num;
	/* 分辨中断源 并调用对应的处理函数 */
	for(i=0;i<31;i++)
		{
			if(VIC0IRQSTATUS_REG & (1<<i))
				{
					irq_array[i](i);	
				}
			if(VIC1IRQSTATUS_REG & (1<<i))
				{
					irq_array[i+32](i+32);	
				}
		}
	
	VIC0ADDRESS_REG  = 0;								//清VIC0ADDRESS
	VIC1ADDRESS_REG  = 0;								//清VIC1ADDRESS
}


