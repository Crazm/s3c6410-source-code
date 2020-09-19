#include "s3c6410.h"
#include "uart.h"

/*  串口0初始化  	  */
void uart0_init(void)     
{
	/*1 配置GPIO为uart引脚 GPACON 0x7F008000 */
	/* GPA0=RXD0 0010 GPA1=TXD0 0010 */
	GPACON_REG &= ~(15|15<<4);	//GPACON[0:7]清零
	GPACON_REG |=  (2|2<<4);	//GPACON[0:7]=00100010

	/* GPA0,GPA1上拉 GPA[n]=[2n+1：2n] 10=上拉使能 */
	GPAPUD_REG &= ~(3|3<<2);	//GPAPUD[0:3]清零
	GPAPUD_REG |=  (2|2<<2);	//GPAPUD[0:3]=1010

	/*2	设置uart0波特率 */
	/*  UART 控制寄存器UCON0[0:3]=0101 中断请求或轮询模式 */
	/*	DIV_VAL = (PCLK / (b/s × 16) ) -1  */
	/*	DIV_VAL = (66.5M / (115200*16)) -1 = 35.078 */
	/*	DIV_VAL=UBRDIV0(整数部分)+UDIVSLOT0(小数部分)/16 */
	UCON0_REG 	|=  (1|1<<2);	// 时钟：PCLK，禁止中断，使能UART发送、接收 
	UBRDIV0_REG  = 	35;			//UBRDIV0_REG=35
	UDIVSLOT0_REG 	 = 	0x1;	//UDIVSLOT0=1

	/*3	设置数据格式	8n1:8个数据位，无奇偶校验，1个停止位	*/
	ULCON0_REG   =  3;		// 数据位:8, 无校验, 停止位: 1, 8n1
	//UFCON0_REG = 0x01; 			// FIFO ENABLE
	UMCON0_REG = 0;				// 无流控
	
}

/*	发送一个字符	*/
void putchar(unsigned char c)
{
	while(!(UTRSTAT0_REG & (1<<2) )); 	  // 如果发送缓冲器未空，等待 
	UTXH0_REG = c ;                      // 发数据 
}

/*	接收一个字符	*/
unsigned char getchar(void)
{
	while(!(UTRSTAT0_REG & (1<<0) )); 	  // 如果接收缓冲器空，等待 
	return (unsigned char)URXH0_REG ;     // 写数据 
}

/*	发送字符串    	*/

void puts(const char *s)
{
	while(*s)
		{
			putchar(*s);
			s++;
		}
}




