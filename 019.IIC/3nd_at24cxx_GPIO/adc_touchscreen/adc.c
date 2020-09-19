
#include "../s3c6410.h"


void adc_init(unsigned char channel)
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

    ADCCON_REG = (1<<16) | (1<<14) | (49<<6) | (channel << 3);
    ADCCON_REG &= ~((1<<2)|(1<<1));

    /* ADCDLY ADC 启动或时间延迟寄存器
     *[16]   :ADCDLY 时钟初始化 0 =外部输入时钟 1 =RTC 时钟
     *[15:0] :DELAY (1)正常转换模式,模式的 XY 坐标,自动模式.→ ADC 转换启动延迟的值.  
     *        (2)等待中断方式.  在 STOP 模式下,触摸笔向下移动发生时,它产生唤醒信 号,
     *        使间隔时间(数毫秒)用于额外的停止模式.  不要使用非零值( 0x0000 ).
     */

    ADCDLY_REG = 0xff;
}

int adc_read(unsigned char channel)
{
    /* adc初始化 */
    adc_init(channel);
    
    /* 启动adc */
    ADCCON_REG |= (1<<0);

    /* 等待ADC结束 */
    while(!(ADCCON_REG & (1<<15)));

    return (ADCDAT0_REG & 0xfff);
    
}

