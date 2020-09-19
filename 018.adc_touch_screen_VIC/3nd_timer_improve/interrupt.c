#include "interrupt.h"
#include "s3c6410.h"
#include "led_on.h"
#include "uart.h"


void register_irq(int irq, irq_func fp)
{
    
	if(irq <32)
	    VIC0INTENABLE_REG  |= (1<<irq);		//使能中断请求，允许中断到达处理器
	else
	    VIC1INTENABLE_REG  |= (1<<(irq-32)); //使能中断请求，允许中断到达处理器

	if(irq <32)
    {
        unsigned int *p = (unsigned int *)(0x71200100 + 4*irq);
        *p = (unsigned int )fp;
    } 
	else
	{   
        unsigned int *p = (unsigned int *)(0x71300100 + 4*(irq-32));
        *p = (unsigned int )fp;
	}
	    
}

/* 中断初始化函数 */
void irq_init(void)
{

	/* 设置中断模式为irq，并向向量地址寄存器赋入口参数地址 */
	VIC0INTSELECT_REG  =  0x0;			//设置irq模式

	/* 在注册中断并使能这些中断 */
	//register_irq(23, timer_irq);
	
}

void handle_irq_c(void)
{

	void (*the_isr)(void);

	if (VIC0IRQSTATUS_REG)
	{
		the_isr = (irq_func)VIC0ADDRESS_REG;
			
		/* 2.1 分辨是哪个中断 */
		/* 2.2 调用它的处理函数 */	
		/* 2.3 清中断 */	

		the_isr();
		
		VIC0ADDRESS_REG = 0;
	}
	else if (VIC1IRQSTATUS_REG)
	{
		the_isr = (irq_func)VIC1ADDRESS_REG;
			
		/* 2.1 分辨是哪个中断 */
		/* 2.2 调用它的处理函数 */	
		/* 2.3 清中断 */	

		the_isr();
		VIC1ADDRESS_REG = 0;
	}
}


