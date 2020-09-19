
int main(void)
{
	unsigned int *pGPKCON1=0x7F008800; //GPK端口输入输出模式寄存器
	unsigned int *pGPKDAT=0x7F008808; //GPK端口输入输出数据寄存器
	*pGPKCON1 = 0x10000000;//配置GPK7为输出引脚
	*pGPKDAT = 0; //GPK端口全部输出0；
	return 0;
}
