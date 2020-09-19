#include "s3c6410.h"


void delay(volatile unsigned int t);//函数声明

int main(void)
{
	unsigned int i=0,j; 
	GPKCON0_REG &=~((15<<16)|(15<<20)|(15<<24)|(15<<28));//GPK4,5,6,7寄存器清零
	GPKCON0_REG |= ((1<<16)|(1<<20)|(1<<24)|(1<<28));//配置GPK4,5,6,7为输出引脚
	GPNCON_REG &=~((3<<0)|(3<<2)|(3<<4)|(3<<6));//GPN1,2,3,4寄存器清零
	GPNCON_REG |= ((2<<0)|(2<<2)|(2<<4)|(2<<6));//配置GPN1,2,3,4为输入引脚
	while(1)
		{
		 if(GPNDAT_REG & (1<<0)) //K1未按下
			 GPKDAT_REG |=  (1<<4);
		 else
			 GPKDAT_REG &= ~(1<<4);
		 if(GPNDAT_REG & (1<<1)) //K2未按下
			 GPKDAT_REG |=  (1<<5);
		 else
			 GPKDAT_REG &= ~(1<<5);
		 if(GPNDAT_REG & (1<<2)) //K3未按下
			 GPKDAT_REG |=  (1<<6);
		 else
			 GPKDAT_REG &= ~(1<<6);
		 if(GPNDAT_REG & (1<<3)) //K4未按下
			 GPKDAT_REG |=  (1<<7);
		 else
			 GPKDAT_REG &= ~(1<<7);
		 
		 
		}
	
	return 0;
}


/* 延时 */	
void delay(volatile unsigned int t)
{
	while(t--);
}



