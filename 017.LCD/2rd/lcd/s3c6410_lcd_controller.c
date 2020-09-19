#include "lcd.h"
#include "lcd_controller.h"
#include "../s3c6410.h"
#include "../my_printf.h"
#define HCLK    133

void s3c6410_lcd_pin_init(void)
{
    /* lcd专用引脚初始化 */
    GPICON_REG = 0xaaaaaaaa;
    GPJCON_REG = 0xaaaaaa;

    /* 其他引脚初始化 */
    GPECON_REG |= 0x1;           /* xpwmTOUT1初始化 */
    GPFCON_REG |= (0x01<<30);    /* EINT10初始化 */

    /* 测试引脚 */
    GPKCON0_REG &=~((15<<16)|(15<<20)|(15<<24)|(15<<28));//GPK4,5,6,7寄存器清零
	GPKCON0_REG |= ((1<<16)|(1<<20)|(1<<24)|(1<<28));//配置GPK4,5,6,7为输出引脚
	GPKDAT_REG	|= ((1<<4)|(1<<5)|(1<<6)|(1<<7));	//默认灯灭

}

/* 根据传入的LCD参数设置LCD控制器 */
void s3c6410_lcd_controller_init(p_lcd_params plcdparams)
{

    /* lcd相关引脚初始化 */
    s3c6410_lcd_pin_init();
    
    /* 在常规显示模式下，必须将SEL_BYPASS@MIFPCON[3](0x7410_800C)
    寄存器设置为 0 */
    MIFPCON_REG &= ~(1<<3);
    printf("MIFPCON:%8x\n\r",MIFPCON_REG);

    /* SPCON:LCD_SEL[1:0]value@0x74F0081A0 必须设置为’00’，
    使用主机 I/F 类型，或者设置为‘01’使用 RGB I/F 类型。 */
    SPCON_REG |= (0x01<<0);
    printf("SPCON_REG:%8x\n\r",SPCON_REG);

    /* VIDCON0:配置视频输出格式和显示使能/禁止 
     * [18:17]:PNRMODE :00RGB 调色板格式（RGB）
     * [13:6] :VCLK=视频时钟源/（CLKVAL+1） CLKVAL>=1
     *         视频时钟源选择HCLK = 133MHZ 取VCLK = 8.87MHZ  
     *          CLKVAL = HCLK/VCLK -1 = 14 
     * [4]    :CLKDIR:0=直接选择时钟源: 1=用 CLKVAL_F 寄存器划分时钟源
     */
    int clkval = 14 ; //(double)HCLK / plcdparams->time_seq.vclk -1 ;
    VIDCON0_REG = (0x0 << 17)  | (clkval << 6) | (1<<4);
    printf("VIDCON0_REG:%8x\n\r",VIDCON0_REG);

    /* VIDCON1: 配置RGB I/F 控制信号
     * [7]    ：IVCLK 此位控制 VCLK 活动边沿的极性： 0下降沿 1上升沿
     * [6]    ：IHSYNC此位指明 HSYNC 脉冲极性: 0=常规 1=反向
     * [5]    : IVSYNC此位指明 VSYNC 脉冲极性:          0=常规 1=反向
     * [4]    : IVDEN此位指明 VDEN 信号极性:  0=常规 1=反向
     */
     VIDCON1_REG = (plcdparams->pins_pol.vclk << 7 ) |  \
                   (plcdparams->pins_pol.hsync << 6 )|  \
                   (plcdparams->pins_pol.vsync << 5 )|  \
                   (plcdparams->pins_pol.de << 4 );
    printf("VIDCON1_REG:%8x\n\r",VIDCON1_REG);
                   
    /* VIDTCON0:配置视频输出时序
     * [23:16] :VBPD(tvb-1) 在帧开始处，垂直后沿是不活动行的数量，在垂直同步过程之后 
     * [15:8]  :VFPD(tvf-1) 在帧末尾处，垂直前沿是不活动行的数量，在垂直同步过程之前
     * [7:0]   :VSPW(tvpw-1) 通过计算不活动行的数量，垂直同步脉冲宽度决定VSYNC 脉冲的高点品宽度
     */
     VIDTCON0_REG = ((plcdparams->time_seq.tvb -1 )<< 16) |      /* 上边黑框, Vertical Back porch */  
                    ((plcdparams->time_seq.tvf -1 )<< 8)  |      /* 下边黑框, Vertical Front porch */
                    ((plcdparams->time_seq.tvp -1 )<< 0);        /* vysnc脉冲宽度 */
    printf("VIDTCON0_REG:%8x\n\r",VIDTCON0_REG);

    /* VIDTCON1:配置视频输出时序
     * [23:16] :HBPD(thb-1) 水平后沿是 HSYNC 下降沿和有效数据开始之间的 VCLK 周期的数量 
     * [15:8]  :HFPD(thf-1) 水平前沿是有效数据末端和 HSYNC 上升沿之间的 VCLK 周期的数量
     * [7:0]   :HSPW(thpw-1) 通过计算不 VCLK 的数量，水平同步脉冲宽度决定 HSYNC 脉冲的高点品宽度
     */
     VIDTCON1_REG = ((plcdparams->time_seq.thb -1 )<< 16) |      /* 左边黑框, Horizontal Back porch */
                    ((plcdparams->time_seq.thf -1 )<< 8)  |      /* 右边黑框, Horizontal Front porch */
                    ((plcdparams->time_seq.thp -1 )<< 0) ;       /* hsync脉冲宽度 */
    printf("VIDTCON1_REG:%8x\n\r",VIDTCON1_REG);

    /* VIDTCON2:配置显示尺寸
     * [21:11] :LINEVAL=垂直显示尺寸-1 
     * [10:0]  :HOZVAL=水平显示尺寸-1
     */
     VIDTCON2_REG = (( plcdparams->xres - 1) << 0 ) |
                    (( plcdparams->yres - 1) << 11);
    printf("VIDTCON2_REG:%8x\n\r",VIDTCON2_REG);

    /* WINCON0:窗口0格式设置
     * [5:2]  :BPPMODE_F选择 BPP 模式窗口图像:1000=unpacked 18BPP（R:6-G:6-B:6）
     * [0]    :ENWIN_F 视频输出和逻辑瞬态使能1/禁止0
     */
     int bppmode = plcdparams ->bpp ==8  ? 0x3:\
                   plcdparams ->bpp ==16 ? 0x5:\
                   plcdparams ->bpp ==18 ? 0x8:\
                        0xb;/* 0xb 24BPP */
                       
     WINCON0_REG = (bppmode <<2) | (0x1 << 0);

     
     printf("WINCON0_REG:%8x\n\r",WINCON0_REG);

     /* VIDOSD0A:配置视频窗口 0 的位置控制寄存器
      * [21:11] :左上角的 OSD 图像像素横向屏幕坐标
      * [10:0]  :左上角的 OSD 图像像素纵向屏幕坐标
      */
     VIDOSD0A_REG = (0 <<11) | (0 <<0);
     printf("VIDOSD0A_REG:%8x\n\r",VIDOSD0A_REG);

     /* VIDOSD0B:配置视频窗口 0 的位置控制寄存器
      * [21:11] :右下角的 OSD 图像像素横向屏幕坐标
      * [10:0]  :右下角的 OSD 图像像素纵向屏幕坐标
      */
     VIDOSD0B_REG = (plcdparams->xres <<11) | (plcdparams->yres <<0);
     printf("VIDOSD0B_REG:%8x\n\r",VIDOSD0B_REG);

    /* VIDOSD0C:配置视频窗口 0 的位置控制寄存器
     * [23:0]  :窗口尺寸=高*宽（字数）
     */
     VIDOSD0C_REG =  plcdparams->xres * plcdparams->yres;
    printf("VIDOSD0C_REG:%8x\n\r",VIDOSD0C_REG);

    /* VIDW00ADD0B0:配置帧缓冲起始地址 0 寄存器
     */
     VIDW00ADD0B0_REG = plcdparams->fb_base;
    printf("VIDW00ADD0B0_REG:%8x\n\r",VIDW00ADD0B0_REG);

    /* VIDW00ADD1B0:配置帧缓冲结束地址 0 寄存器
     *[23:0] :这些位指明视频帧缓冲区的结束地址 A[23:0]
     * VBASEL=VBASEU+(PAGEWIDTH+OFFSIZE)*(LINEVAL+1) ?????
     */
     
     VIDW00ADD1B0_REG = plcdparams->fb_base + ((plcdparams->xres * \
                        plcdparams->yres * 4) & (0xffffff));   /* 结束地址=起始地址+行*列*每个像素的字节数 */
     printf("VIDW00ADD1B0_REG:%8x\n\r",VIDW00ADD1B0_REG);

     VIDW00ADD2_REG =  plcdparams->xres; 
     
}

void s3c6410_lcd_controller_enable(void)
{
    /* VIDCON0:配置视频输出格式和显示使能/禁止 
     * [1]    :ENVID 视频输出和逻辑瞬态使能0/禁止1 
     * [0]    :ENVID_F在当前帧末尾视频输出和逻辑瞬态使能0/禁止1
     */
      VIDCON0_REG |= 0x3 ;

    /* 打开背光灯 */
    GPEDAT_REG |= 0x1;

     
}

void s3c6410_lcd_controller_disable(void)
{
    /* VIDCON0:配置视频输出格式和显示使能/禁止 
     * [1]    :ENVID 视频输出和逻辑瞬态使能0/禁止1 
     * [0]    :ENVID_F在当前帧末尾视频输出和逻辑瞬态使能0/禁止1
     */
     VIDCON0_REG &= (~0x3);

    /* 关闭背光灯 */
    GPEDAT_REG = 0;

    
    /* 测试 */
    GPKCON0_REG &=~((15<<16)|(15<<20)|(15<<24)|(15<<28));//GPK4,5,6,7寄存器清零
	GPKCON0_REG |= ((1<<16)|(1<<20)|(1<<24)|(1<<28));//配置GPK4,5,6,7为输出引脚
}

struct lcd_controller s3c6410_lcd_controller = {
    .name    = "s3c6410",
    .init    = s3c6410_lcd_controller_init,
    .enable  = s3c6410_lcd_controller_enable,
    .disable = s3c6410_lcd_controller_disable,

};

void s3c6410_lcd_contoller_add(void)
{
	register_lcd_controller(&s3c6410_lcd_controller);
}

