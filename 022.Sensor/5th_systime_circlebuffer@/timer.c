#include "s3c6410.h"
#include "timer.h"
#include "interrupt.h"
#include "include/stdio.h"

#define TIMER_NUM  32

static unsigned long long g_system_10ms = 0;

timer_desc timer_array[TIMER_NUM];

int register_timer(char *name, timer_func fp)
{
    int i;
    for(i=0; i<TIMER_NUM; i++)
    {
        if(!timer_array[i].fp)
          {
            timer_array[i].name = name;
            timer_array[i].fp   = fp;
            return 0;
          }
    }
    return -1;
}

extern int strcmp(const char * cs,const char * ct);

int unregister_timer(char *name)
{
    int i;
    for(i=0; i<TIMER_NUM; i++)
    {
        if(!strcmp(timer_array[i].name,name))
          {
            timer_array[i].name = NULL;
            timer_array[i].fp   = NULL;
            return 0;
          }
    }
    return -1;
}

/* 定时器中断服务程序 */

void timer_irq(void)
{
	int i;
	g_system_10ms ++;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (timer_array[i].fp)
		{
			timer_array[i].fp();
		}
	}	

    TINT_CSTAT_REG |= (1<<5);   /*  清定时器0中断 */
}


/*定时器中断初始化 */
void timer0_init(void)
{
	
	/* 设置时钟 */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	 *        = 66500000/(132+1)/1
	 *        = 500000hz
	 */
	TCFG0_REG  &=  ~(255<<0);
	TCFG0_REG  |=   (132<<0);	/* Prescaler 0 = 132, 用于timer0,1 */

	TCFG1_REG  &=  ~(15<<0);
	TCFG1_REG  |=   (0<<0);		/*  MUX0 : 1/1 */
	
	/* 设置初值(定时时间) */
	TCNTB0_REG = 5000;  	    /* 定时10ms */	
	TCMPB0_REG = 0;

	/* 加载初值 */
	TCON_REG   |=   (1<<1);		/* 手动更新 TCNTB0 或 TCMPB0  即加载初值 */
	TCON_REG   &=   ~(1<<1);	/* 清手动更新位 */
	
	/* 设置为自动加载并启动定时器0 */
	TCON_REG   |=   (1<<3);		/* 设置为自动加载 */
	TCON_REG   |=	(1<<0); 	/* 启动定时器 */

	/* 注册定时器中断 */
	register_irq(23, timer_irq);
	
	TINT_CSTAT_REG |= (1<<0);   /* 使能定时器0中断 */
	
}

void timer0_pwm(unsigned int frq, unsigned duty_cycle)
{
	unsigned int tcnb0,tcmpb0;

	/* 设置GPF14为PWM0输出引脚 */
	GPFCON_REG |=   (2<<28);	//设置GPF14为PWM TOUT[0]输出引脚

	/* 关闭定时器0 */
	TCON_REG   &=   ~(15<<0);	/* 关闭定时器0 */

	/*设置频率，占空比换算 */
	tcnb0 = 5000/frq;
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


void udelay(int us)
{
  
  int cnt = us/2;
  int pre = TCNTO0_REG;
  int cur,sum;
  while(cnt > 0 )
  {
    cur = TCNTO0_REG;
    if(cur <= pre)
       sum = pre - cur;
    else
       sum = pre + 5000 -cur;
    cnt -= sum;
    pre = cur;
  }
  
}

void mdelay(int ms)
{
   udelay(ms*1000); 
}

unsigned long long get_system_time_us(void)
{
    unsigned int us = (5000 - TCNTO0_REG)*2;
    return ( g_system_10ms * 10 * 1000 + us);
}

unsigned int delta_time_us(unsigned long long pre, unsigned long long now)
{
    return (now - pre);
}


void delay_test(void)
{
   int time = 0;
   while(1)
   {
    printf("time:");
    mdelay(1000);
    time++;
    printf("%d\n\r",time);
   }
}


