
#include "s3c6410.h"
#include "led_on.h"
#include "uart.h"
#include "timer.h"


/* 中断初始化函数 */
void irq_init(void)
{

	/* 设置中断模式为irq，并向向量地址寄存器赋入口参数地址 */
	VIC0INTSELECT_REG  =  0x0;			//设置irq模式

	/* 在中断控制器里使能这些中断 */
	VIC0INTENABLE_REG  |= (1<<23);		//使能中断请求，允许中断到达处理器
	
}

void handle_irq_c(void)
{
	unsigned int i;
	/* 分辨中断源 并调用对应的处理函数 */
	timer_irq(); /* 处理中断 */	
	
	unsigned int uTmp;
	
	uTmp = TINT_CSTAT_REG;
	TINT_CSTAT_REG = uTmp;     							//清timer0的中断状态寄存器 
	VIC0ADDRESS_REG  = 0;								//清VIC0ADDRESS
	
}


