/* 选择要亮的灯 */
unsigned int led_switch(unsigned char i)
{
	unsigned int pGPKDAT;
	if(i==1)
		{
			pGPKDAT = 0x000000e0; //GPK4端口输出0
		}
	if(i==2)
		{
			pGPKDAT = 0x000000d0; //GPK5端口输出0
		}
	if(i==3)
		{
			pGPKDAT = 0x000000b0; //GPK6端口输出0
		}
	if(i==4)
		{
			pGPKDAT = 0x00000070; //GPK7端口输出0
		}
	return pGPKDAT;
}

/* 延时 */	
void delay(volatile unsigned int t)
{
	while(t--);
}

int main(void)
{
	unsigned int *pGPKCON1=0x7F008800; //GPK端口输入输出模式寄存器
	unsigned int *pGPKDAT=0x7F008808; //GPK端口输入输出数据寄存器
	*pGPKCON1 = 0x11110000;//配置GPK4,5,6,7为输出引脚
	while(1)
		{
		*pGPKDAT=led_switch(1);
		delay(10000);
		*pGPKDAT=led_switch(2);
		delay(10000);
		*pGPKDAT=led_switch(3);
		delay(10000);
		*pGPKDAT=led_switch(4);
		delay(10000);
		}
	
	return 0;
}




