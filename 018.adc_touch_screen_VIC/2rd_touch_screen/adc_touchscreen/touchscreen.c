#include "../s3c6410.h"
#include "../my_printf.h"
#include "../interrupt.h"
#include "../led_on.h"

static int x, y;
static volatile unsigned char has_get_xy = 0;

void wait_for_pen_down(void)
{
    /* 让触摸屏控制器进入“等待中断模式” */
	ADCTSC_REG = 0xd3;	
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

/* 触摸笔按下抬起中断服务程序 */
void irq_pen_down_up(void)
{
	if (ADCDAT0_REG & (1<<15))
	{
		printf("pen up\n\r");
		
		wait_for_pen_down();
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

/* ADC读触摸屏坐标中断服务程序 */
void irq_adc(void)
{
	if ((ADCDAT0_REG & (1<<15)) == 0) /* 触摸笔仍然处于按下状态 */
	{
		/* 如果是x坐标, 记录下来, 启动Y坐标的ADC */
		if ((ADCTSC_REG & 0x3) == 1)
		{
			x = ADCDAT0_REG & 0xfff;
			/* 清中断 */
			ADCCLRINT_REG = 0;
			enter_to_measure_y();
			start_adc();
		}
		else if ((ADCTSC_REG & 0x3) == 2)
		{
			/* 如果是Y坐标, 记录下来, wait_for_pen_up */
			y = ADCDAT1_REG & 0xfff;
			/* 清中断 */
			ADCCLRINT_REG = 0;
			wait_for_pen_up();
			printf("[x,y] = [%d, %d]\n\r", x, y);
		}
	}
	else
	{
		/* 在ADC期间, 触摸笔已经松开了 */
		wait_for_pen_down();
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

    /* 让触摸屏控制器进入“等待中断模式” */
    wait_for_pen_down(); 
    
}

