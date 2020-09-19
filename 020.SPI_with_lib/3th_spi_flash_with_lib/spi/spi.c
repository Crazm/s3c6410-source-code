#include "../s3c6410.h"
#include "../include/stdio.h"

/* 设置GPIO为SPI专用 */
static void SPI_GPIO_Init(void)
{
    /* GPC4 MISO输入，GPC5 CLK输出，
     * GPC6 MOSI输出，GPC7 CS 输出 
     */
    GPCCON_REG =(2<<28)|(2<<24)|(2<<20)|(2<<16);
    GPCPUD_REG =0xaa00;  //使能上拉
}



void SPISendByte(unsigned char val)
{ 
 
    SLAVE1_SLECTION_REG &= ~(1<<0);     //片选信号使能
    
    SPI1_TX_DATA_REG = val;
    printf("tx   val:%x\n\r",val);

    while(((SPI1_STATUS_REG >> 6) & 0x7f ));

    printf("SPI1_STATUS_TX:%8x\n\r",SPI1_STATUS_REG);

    SLAVE1_SLECTION_REG |= (1<<0);     
    printf("SPI1_STATUS_TX:%8x SLECTION =1\n\r",SPI1_STATUS_REG);
}

unsigned char SPIReceByte(void)
{   
   unsigned char val;
   SLAVE1_SLECTION_REG &= ~(1<<0);     //片选信号使能

   printf("SPI1_STATUS_RX:%8x\n\r",SPI1_STATUS_REG);

   while(((SPI1_STATUS_REG >> 13) & 0x7f ) != 1 );

   val = SPI1_RX_DATA_REG & 0xff;
   printf("recv val:%x\n\r",val);

   SLAVE1_SLECTION_REG |= (1<<0); 
    
   return val;
    
           
}


void SPIInit(void)
{
    SPI_GPIO_Init();    //GPIO初始化 

   /*CH_CFG SPI配置寄存器
    *[4]:SPI通道属性: 0: 主控器1: 从属器
    *[3]:CPOL: 0: 有效的高位 1:有效的低位
    *[2]:CPHA: 0: 格式A 1: 格式 B
    *[1]:SPI 接收通道:0: 通道关闭 1: 通道打开
    *[0]:SPI 发送通道:0: 通道关闭 1: 通道打开
    */
    CH1_CFG_REG = (0<<4)|(1<<3)|(1<<2);
    CH1_CFG_REG &= ~(1<<6);

    /*Clk_CFG 时钟配置寄存器
     *[10:9] :选择时钟源:00 : PCLK
     *[8]    :时钟开/关: 0 : 无效 1 : 有效
     *[7:0]  :SPI 时钟输出分频频率
     *SPI 时钟输出=时钟源 / ( 2 ×(预分频值 +1))
     *SPI_CLK = 65.5M/(2*(0+1))=32.75MHZ
     */
     Clk1_CFG_REG = (0<<9)|(1<<8)|(9<<0);

     /*MODE_CFG  SPI FIFO控制寄存器
      *[30:29]: Ch_tran_size:00 : 字节 1 : 半字 10 :字 11 : 保留
      *[28:19]: Trailing Count:从接收FIFO中写入的最后数据到覆盖FIFO中结尾的字节
      *[18:17]: BUS transfer size:字节 1 : 半字 10 :字 11 : 保留
      *[16:11]: RxTrigger该值是接收FIFO的字节数
      *[10:5] : TxTrigger该值是发送FIFO的字节数
      *[2]    : RxDMA On :DMA模式:0关闭 ，1打开
      *[1]    : TxDMA On :DMA模式:0关闭 ，1打开
      *[0]    : DMA 传输类型: 0 : 单个 1 : 4 个脉冲
      */
      
      MODE1_CFG_REG = (0<<29)|(1<<19)|(0<<17)|(1<<11)|(1<<5)|\
                      (0<<2) |(0<<1) |(0<<0);

      CH1_CFG_REG |= (1<<1)|(1<<0); //打开SPI发送接收通道

}



 
