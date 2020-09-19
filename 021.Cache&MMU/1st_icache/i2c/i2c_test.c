#include "../include/stdio.h"
#include "../include/string.h"
#include "at24cxx.h"
#include "i2c_controller.h"

void do_read_AT24CXX(void)
{
	unsigned int addr;
	int i, j,err;
	unsigned char str[100];
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	addr = get_uint();
    if(addr >1024)
	{   
	    printf("address > 1024,please try again\n\r");
	    return;
	}
	
	err = at24cxx_read(addr,str,64);
	printf("at24cxx_read_return:%d \n\r",err);

	printf("Data : \n\r");
	/* 长度固定为64 */
	for (i = 0; i < 4; i++)
	{
		/* 每行打印16个数据 */
		for (j = 0; j < 16; j++)
		{
			/* 先打印数值 */
			printf("%02x ", str[i*16+j]);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* 后打印字符 */
			if (str[i*16+j] < 0x20 || str[i*16+j] > 0x7e)  /* 不可视字符 */
				putc('.');
			else
				putc(str[i*16+j]);
		}
		printf("\n\r");
	}
}

void do_write_AT24CXX(void)
{
	unsigned int addr;
	unsigned char str[100];
	int err;
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();
	if(addr >1024)
	{   
	    printf("address > 1024,please try again\n\r");
	    return;
	}
	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");
	err = at24cxx_write(addr, str, strlen(str)+1);
	printf("at24cxx_write_return:%d \n\r",err);

}



void i2c_test(void)
{
    char c;
    i2c_init();

	while (1)
	{
		/* 打印菜单, 供我们选择测试内容 */
		printf("[w] Write AT24CXX\n\r");
		printf("[r] Read  AT24CXX\n\r");
		printf("[q] quit\n\r");
		printf("Enter selection: ");

		c = getc();
		printf("%c\n\r", c);

		/* 测试内容
		 * 3. 写某个地址
		 * 4. 读某个地址
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				return;
				break;

			case 'w':
			case 'W':
				do_write_AT24CXX();
				break;

			case 'r':
			case 'R':
				do_read_AT24CXX();
				break;
			default:
				break;
		}
	}
}
