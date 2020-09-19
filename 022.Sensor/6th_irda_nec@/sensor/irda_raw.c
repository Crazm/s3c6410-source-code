#include "../s3c6410.h"
#include "../interrupt.h"
#include "irda_raw.h"
#include "circle_buffer.h"
#include "../timer.h"
#include "../include/stdio.h"

static unsigned long long g_last_time = 0;

/* 配置GPIO   */
static void set_io_as_eint(void)
{
    GPNCON_REG &= ~(3<<16);
    GPNCON_REG |=  (2<<16);     //设置为外部中断引脚

    GPNPUD_REG &= ~(3<<16);     //禁止上拉下拉

    EINT0CON0_REG |= (7<<16);   //双边沿触发

    EINT0MASK_REG &= ~(1<<8);   //允许该引脚产生中断
    
}

static int get_data(void)
{
    if(GPNDAT_REG & (1<<8))
         return 1;
    else
         return 0; 
}

/* 在中断处理函数中：
 * 记录中断发生的时间
 * 跟上次中断时间比较，计算出脉冲宽度
 * 读取引脚极性，把数据放入环形缓冲区
 */
void irda_irq(void)
{
    irda_raw_event event;
    unsigned long long cur = get_system_time_us();
    event.duration = delta_time_us(g_last_time, cur);
    event.pol      = !get_data();
    ir_event_put(&event);
    g_last_time = cur;
    
    /* 请中断源 */
    EINT0PEND_REG |= (1<<8); 
}

/* 注册中断 */
void irda_init(void)
{
    set_io_as_eint();
    register_irq(1,irda_irq);  
}

void irda_raw_test(void)
{
    irda_init();
    irda_raw_event event;
    unsigned long long pre = 0,cur;
    while(1)
    {
        if(ir_event_get(&event) == 0)
        {
          cur = get_system_time_us();
          if(delta_time_us(pre, cur) > 1000000)
            printf("\n\r");
          printf("%s %dus | ",event.pol?"high":"low",event.duration);
          pre = cur;
        }
    }
}


