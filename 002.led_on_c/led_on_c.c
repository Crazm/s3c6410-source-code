
int main(void)
{
	unsigned int *pGPKCON1=0x7F008800; //GPK�˿��������ģʽ�Ĵ���
	unsigned int *pGPKDAT=0x7F008808; //GPK�˿�����������ݼĴ���
	*pGPKCON1 = 0x10000000;//����GPK7Ϊ�������
	*pGPKDAT = 0; //GPK�˿�ȫ�����0��
	return 0;
}
