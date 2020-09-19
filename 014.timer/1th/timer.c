#include "s3c6410.h"

/*定时器中断初始化 */
void timer_init(void)
{
	/* 设置时钟 */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	          = 66500000/(99+1)/4
	          = 166250
	 */
	TCFG0_REG  &=  ~(255<<0);
	TCFG0_REG  |=   (99<<0);	/* Prescaler 0 = 99, 用于timer0,1 */

	TCFG1_REG  &=  ~(15<<0);
	TCFG1_REG  |=   (2<<0);		/*  MUX0 : 1/4 */

	/* 设置初值(定时时间) */
	TCNTB0_REG = 83125;  		/* 0.5s中断一次 */
	TCMPB0_REG = 0;

	/* 加载初值 */
	TCON_REG   |=   (1<<1);		/* 手动更新 TCNTB0 或 TCMPB0  即加载初值 */
	TCON_REG   &=   ~(1<<1);		/* 清手动更新位 */
	
	/* 设置为自动加载并启动定时器0 */
	TCON_REG   |=   (1<<3);		/* 设置为自动加载 */
	TCON_REG   |=	(1<<0); 	/* 启动定时器 */

	/* 使能定时器0中断 */
	TINT_CSTAT_REG	|=  (1<<0 );	//[0]置一，启动定时器0中断
	
}

/*定时器中断服务函数 */
void timer_irq(void)
{
	GPKDAT_REG   ^=  (1<<7);    /* led4=~led4*/
}

