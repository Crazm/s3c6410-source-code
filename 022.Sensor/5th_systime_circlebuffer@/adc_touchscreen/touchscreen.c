#include "../s3c6410.h"
#include "../include/stdio.h"
#include "../interrupt.h"
#include "../led.h"
#include "../timer.h"
#include "../lcd/lcd.h"
#include "../lcd/geometry.h"
#include "../lcd/font.h"


static int x, y;
static volatile unsigned char has_get_xy = 0;

static int g_ts_x;
static int g_ts_y;
static int g_ts_pressure;
static int g_ts_data_valid = 0;

void wait_for_pen_down(void)
{
    /* 让触摸屏控制器进入“等待中断模式” */
	ADCTSC_REG = 0xd3;	
	has_get_xy = 0;
}

void wait_for_pen_up(void)
{
	ADCTSC_REG = 0x1d3;

}

void enter_to_measure_x(void)
{
	/* 进入测量x坐标：AUTO_PST=0, XY_PST: 2b01 */
	
	 ADCTSC_REG = 1;
}

void start_adc(void)
{
    /* 启动ADC */
	ADCCON_REG |= (1<<0);
}

void enter_to_measure_y(void)
{
    /* 进入测量y坐标：AUTO_PST=0, XY_PST: 2b10 */
	
	ADCTSC_REG = 2;
}


void report_ts_xy(int x, int y, int pressure)
{

	if (g_ts_data_valid == 0)
	{
		g_ts_x = x;
		g_ts_y = y;
		g_ts_pressure = pressure;
		g_ts_data_valid = 1;
	}
}

void ts_read_raw(int *px, int *py, int *ppressure)
{
	while (g_ts_data_valid == 0);
	*px = g_ts_x;
	*py = g_ts_y;
	*ppressure = g_ts_pressure;
	g_ts_data_valid = 0;
}


/* 触摸笔按下抬起中断服务程序 */
void irq_pen_down_up(void)
{
	if (ADCDAT0_REG & (1<<15))
	{
		printf("pen up\n\r");
		
		wait_for_pen_down();
		report_ts_xy(0, 0, 0);
	}
	else
	{
		printf("pen down\n\r");
		enter_to_measure_x();
		start_adc();
	}

	ADCUPDN_REG = 0;
	ADCCLRWK_REG = 0;
}


/* 每10ms该函数被调用一次 
 */
void touchscreen_timer_irq(void)
{
	/* 如果触摸屏仍被按下, 进入"手动测量模式", 启动ADC */
	if (has_get_xy == 0)
		return;

	if (ADCDAT0_REG & (1<<15)) /* 如果松开 */
	{
		has_get_xy = 0;
		wait_for_pen_down();
		return;
	}
	else  /* 按下状态 */
	{
		/* 进入"测量"模式 */
		enter_to_measure_x();

		/* 启动ADC */
		start_adc();
	}
}


/* ADC读触摸屏坐标中断服务程序 */
void irq_adc(void)
{
    static int adc_cnt = 0;
	static int adc_x = 0;
	static int adc_y = 0;
	
	if ((ADCDAT0_REG & (1<<15)) == 0) /* 触摸笔仍然处于按下状态 */
	{
		/* 如果是x坐标, 记录下来, 启动Y坐标的ADC */
		if ((ADCTSC_REG & 0x3) == 1)
		{
			x = (ADCDAT0_REG & 0xfff);
			/* 清中断 */
			ADCCLRINT_REG = 0;
			enter_to_measure_y();
			start_adc();
		}
		else if ((ADCTSC_REG & 0x3) == 2)
		{
			/* 如果是Y坐标, 记录下来, wait_for_pen_up */
			y = (ADCDAT1_REG & 0xfff);
			/* 清中断 */
			ADCCLRINT_REG = 0;

            adc_x += x;
    		adc_y += y;
    		
    		adc_cnt++;

    		if (adc_cnt == 16)
    		{
    			adc_x >>= 4;
    			adc_y >>= 4;
    			report_ts_xy(adc_x, adc_y, 1);

    			adc_cnt = 0;
    			adc_x = 0;
    			adc_y = 0;
    			
    			/* 启动定时器以再次读取数据 */
    			/* 先设置TS进入"等待中断模式" */
    			wait_for_pen_up();
    			has_get_xy = 1;
    		}
    		else
    		{
    			/* 进入"测量"模式 */
        		enter_to_measure_x();

        		/* 启动ADC */
        		start_adc();
    		}
            
		}
	}
	else
	{
		/* 在ADC期间, 触摸笔已经松开了 */
		wait_for_pen_down();
		has_get_xy = 0;
		//report_ts_xy(0, 0, 0);
	}
}


void adc_ts_int_init(void)
{
    register_irq(62,irq_pen_down_up);
    register_irq(63,irq_adc);
}

void adc_ts_reg_init(void)
{
    /* ADCCON: ADC 控制寄存器
     *[16]   :RESSEL 0 = 10-bit A/D conversion,1 = 12-bit A/D conversion
     *[15]   :ECFLG转换的结束标记（只读）0= A/D 转换的过程中 1= A/D 转换结束
     *[14]   :ADC 预定标器启动 0 =禁用 ,1 =启动
     *[13:6] :PRSCVL 预定标器值 5～255
     *        A/D converter freq = PCLK /(PRSCVL+1)
     *[5:3]  :SEL_MUX 模拟输入通道选择,000 = AIN 0
     *[2]    :STDBM 待机模式选择0 =正常运作模式 1 =待机模式 
     *[1]    :READ_START A/D 转换开始读取 0 =禁用开始读操作 1 =启动开始读操作
     *[0]    :ENABLE_START A/D 转换开始启用 如果 READ_START 启用，这个值是无效的
     *        0 =无行动 1 =A/D 转换开始和该位被清理后开启
     */

    ADCCON_REG = (1<<16) | (1<<14) | (49<<6) | (0<<3) ;
    ADCCON_REG &= ~((1<<2)|(1<<1)|(1<<0));

    /* ADCDLY ADC 启动或时间延迟寄存器
     *[16]   :ADCDLY 时钟初始化 0 =外部输入时钟 1 =RTC 时钟
     *[15:0] :DELAY (1)正常转换模式,模式的 XY 坐标,自动模式.→ ADC 转换启动延迟的值.  
     *        (2)等待中断方式.  在 STOP 模式下,触摸笔向下移动发生时,它产生唤醒信 号,
     *        使间隔时间(数毫秒)用于额外的停止模式.  不要使用非零值( 0x0000 ).
     */

    ADCDLY_REG = 0xffff;
 
}

void touchscreen_init(void)
{
    /* 设置触摸屏寄存器 */
    adc_ts_reg_init();

    /* 设置中断 */
    adc_ts_int_init();

    /* 注册定时器处理函数 */
	register_timer("touchscreen", touchscreen_timer_irq);

    /* 让触摸屏控制器进入“等待中断模式” */
    wait_for_pen_down(); 
    
}


