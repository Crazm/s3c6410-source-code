#include "s3c6410.h"
#include "my_printf.h"
#include "uart.h"
#include "string_utils.h"

/*nand_flash 初始化 */
void nand_init(void)
{
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

	/*设置nand flash的时序 */
	NFCONF_REG = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);        //TACLS=0ns,TWRPH0=15ns,TWRPH1=5ns

	/*使能nand flsah 控制器 */
	NFCONT_REG |=(3<<0);    //使能nandflash控制器，禁止片选
	NFCONT_REG &= ~(1<<16);	//选中Soft Disable lock,禁用锁
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

void nand_w_data(unsigned char val )
{
	NFDATA_REG = val;
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

void wait_ready(void)
{
	while(!(NFSTAT_REG & 1));
}

int nand_bad(unsigned int addr)
{
	unsigned int col  = 2048;
	unsigned int page = addr / 2048;
	unsigned char val;

	/* 1. 选中 */
	nand_select();
	
	/* 2. 发出读命令00h */
	nand_cmd(0x00);
	
	/* 3. 发出地址(分5步发出) */
	/* col addr */
	nand_addr_byte(col & 0xff);
	nand_addr_byte((col>>8) & 0xff);
	 
	/* row/page addr */
	nand_addr_byte(page & 0xff);
	nand_addr_byte((page>>8) & 0xff);
	nand_addr_byte((page>>16) & 0xff);
	
	/* 4. 发出读命令30h */
	nand_cmd(0x30);
	
	/* 5. 判断状态 */
	wait_ready();

	/* 6. 读数据 */
	val = nand_data();
	
	/* 7. 取消选中 */		
	nand_deselect();


	if (val != 0xff)
		return 1;  /* bad blcok */
	else
		return 0;
}


void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int i = 0 ;
	int page = addr /2048;
	int col = addr %2048; 
	
	while(i < len)
	{
		if (!(addr & 0x1FFFF) && nand_bad(addr)) /* 一个block只判断一次 */
				{
					page += 64;  /* 跳过当前block */
					continue;
				}
		/* 使能片选 */
		nand_select();			
		
		/*发出 00h 命令*/
		nand_cmd(0x00);

		/*发出 地址*/
		/* col addr */
		nand_addr_byte(col & 0xff);
		nand_addr_byte((col>>8) & 0xff);
 
		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff);
		
		/*发出 30h 命令*/
		nand_cmd(0x30);
		
		/* 等待就绪 */
		wait_ready();
		
		/* 读数据 */
		for(; (col <2048) && (i < len); col++)
		{
			buf[i++] = nand_data();
		}
		if (i == len)	break;
		col = 0;
		page++;
	}
	nand_deselect();
}

void do_read_nand_flash(void)
{
	unsigned int addr;
	int i, j;
	unsigned char str[64];
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	addr = get_uint();
	nand_read(addr,str,64);

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

int nand_erase(unsigned int addr, unsigned int len)
{
	int page = addr /2048;

	if(addr % (1024*128))
		{
			printf("nand erase error,addr is not block align\n\r ");
			return -1;
		}
	if(len % (1024*128))
		{
			printf("nand erase error,len is not block align\n\r ");
			return -1;
		}
	
	nand_select();
	while (1)
	{
		nand_cmd(0x60);
		
		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff);

		nand_cmd(0xD0);

		wait_ready();

		len -= (128*1024);
		if(len == 0)
			break;
	}
	return 0;
}

void nand_write(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int i = 0 ;
	int page = addr /2048;
	int col = addr %2048; 
	
	while(1)
	{
		if (!(addr & 0x1FFFF) && nand_bad(addr)) /* 一个block只判断一次 */
				{
					page += 64;  /* 跳过当前block */
					continue;
				}
		
		/* 发送命令0x80 */
		nand_select();			

		/* 使能片选 */
		nand_cmd(0x80);	
		
		/*发出 地址*/
		/* col addr */
		nand_addr_byte(col & 0xff);
		nand_addr_byte((col>>8) & 0xff);
 
		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff);

		/*发出数据 */
		for( ; (col <2048) && (i<len);)
		{
			nand_w_data(buf[i++]);
		}

		nand_cmd(0x10);

		wait_ready();

		if (i == len)	break;
		else
		{
			/* 开始下一个循环page */
			col = 0;
			page ++;			
		}
	}
	
	nand_deselect();
	
}

void do_erase_nand_flash(void)
{
	unsigned int addr;
	
	/* 获得地址 */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");

	nand_erase(addr, 1024*128);
}

void do_write_nand_flash(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");
	nand_write(addr, str, strlen(str)+1 );

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
				do_erase_nand_flash();
				break;

			case 'w':
			case 'W':
				do_write_nand_flash();
				break;

			case 'r':
			case 'R':
				do_read_nand_flash();
				break;
			default:
				break;
		}
	}
}

