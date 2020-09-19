#include "s3c6410.h"
#include "my_printf.h"
#include "uart.h"

/*nand_flash 初始化 */
void nand_init(void)
{
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  1

	/*设置nand flash的时序 */
	NFCONF_REG = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);        //TACLS=0ns,TWRPH0=25ns,TWRPH1=10ns

	/*使能nand flsah 控制器 */
	NFCONT_REG |=(3<<0);    //使能nandflash控制器，禁止片选
}

void nand_select(void)
{
	NFCONT_REG &= ~(1<<1); 	//使能片选
}

void nand_deselect(void)
{
	NFCONT_REG |= (1<<1); 	//禁止片选
}

void nand_cmd(unsigned char cmd)
{
	volatile int i;
	NFCMMD_REG = cmd;
	for(i=0;i<5;i++);
}

void nand_addr_byte(unsigned char addr)
{
	volatile int i;
	NFADDR_REG = addr;
	for(i=0;i<5;i++);
}

unsigned char nand_data(void)
{
	return NFDATA_REG;
}

/* nand_chip_ id */
void nand_chip_id(void)
{
	unsigned char buf[5] = {0};
	nand_select(); 
	nand_cmd(0x90);
	nand_addr_byte(0x00);
	buf[0] = nand_data();
	buf[1] = nand_data();
	buf[2] = nand_data();
	buf[3] = nand_data();
	buf[4] = nand_data();
	nand_deselect();

	printf("manufa		id  = 0x%x\n\r",buf[0]);
	printf("device		id  = 0x%x\n\r",buf[1]);
	printf("3rd byte	id  = 0x%x\n\r",buf[2]);
	printf("4th byte	id  = 0x%x\n\r",buf[3]);
	printf("5th byte	id  = 0x%x\n\r",buf[4]);
	printf("page size 	    = %d KB\n\r",1<<(buf[3]&0x3));
	printf("block size 	    = %d KB\n\r",64<<((buf[3]>>4)&0x3));

	
}

void nand_flash_test(void)
{
	char c;

	while (1)
	{
		/* 打印菜单, 供我们选择测试内容 */
		printf("[s] Scan nand flash\n\r");
		printf("[e] Erase nand flash\n\r");
		printf("[w] Write nand flash\n\r");
		printf("[r] Read nand flash\n\r");
		printf("[q] quit\n\r");
		printf("Enter selection: ");

		c = getc();
		printf("%c\n\r", c);

		/* 测试内容:
		 * 1. 识别nand flash
		 * 2. 擦除nand flash某个扇区
		 * 3. 编写某个地址
		 * 4. 读某个地址
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				return;
				break;
				
			case 's':
			case 'S':
				nand_chip_id();
				break;

			case 'e':
			case 'E':
				break;

			case 'w':
			case 'W':
				break;

			case 'r':
			case 'R':
				break;
			default:
				break;
		}
	}
}

