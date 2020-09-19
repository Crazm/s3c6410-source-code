#include "s3c6410.h"

/*定时器中断初始化 */
void timer0_pwm_init(void)
{
	/* 设置GPF14为PWM0输出引脚 */
	GPFCON_REG |=   (2<<28);	//设置GPF14为PWM TOUT[0]输出引脚

	/* 设置时钟 */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	          = 66500000/(132+1)/1
	          = 500000hz
	 */
	TCFG0_REG  &=  ~(255<<0);
	TCFG0_REG  |=   (132<<0);	/* Prescaler 0 = 132, 用于timer0,1 */

	TCFG1_REG  &=  ~(15<<0);
	TCFG1_REG  |=   (0<<0);		/*  MUX0 : 1/1 */
	
}

void timer0_pwm(unsigned int frq, unsigned duty_cycle)
{
	unsigned int tcnb0,tcmpb0;

	/* 关闭定时器0 */
	TCON_REG   &=   ~(15<<0);	/* 关闭定时器0 */

	/*设置频率，占空比换算 */
	tcnb0 = 500000/frq;
	tcmpb0 = tcnb0 - tcnb0/100*duty_cycle;
	
	/* 设置初值(定时时间) */
	TCNTB0_REG = tcnb0;  		
	TCMPB0_REG = tcmpb0;

	/* 加载初值 */
	TCON_REG   |=   (1<<1);		/* 手动更新 TCNTB0 或 TCMPB0  即加载初值 */
	TCON_REG   &=   ~(1<<1);		/* 清手动更新位 */
	
	/* 设置为自动加载并启动定时器0 */
	TCON_REG   |=   (1<<3);		/* 设置为自动加载 */
	TCON_REG   |=	(1<<0); 	/* 启动定时器 */
}


