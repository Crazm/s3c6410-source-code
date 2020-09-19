#include "spi.h"
#include "../s3c6410.h"
#include "../include/stdio.h"
#include "../include/string.h"



static void SpiFlashSendAddr(unsigned int addr)
{
    SPISendByte( (addr >> 16) & 0xff );
    SPISendByte( (addr >> 8 ) & 0xff );
    SPISendByte( addr & 0xff ); 
}

/* 读Id */
void SpiFlashReadId(int *pMid,int *pDid)
{
    
    SPISendByte(0x90);
    SpiFlashSendAddr(0);
    *pMid = SPIReceByte();
    *pDid = SPIReceByte(); 

}

static void SPIFlashWriteEnable(int enable)
{
    if (enable)
    {
        
        SPISendByte(0x06);
        
    }
    else
    {
        
        SPISendByte(0x04);
        
    }
}

static unsigned char SPIFlashReadStatusReg1(void)
{
    unsigned char val;
   
    SPISendByte(0x05);
    val = SPIReceByte();
    
    return val;
}

static unsigned char SPIFlashReadStatusReg2(void)
{
    unsigned char val;
    
    SPISendByte(0x35);
    val = SPIReceByte();
   
    return val;
}

static void SPIFlashWaitWhenBusy(void)
{
    while (SPIFlashReadStatusReg1() & 0x1);
}

static void SPIFlashWriteStatusReg(unsigned char reg1, unsigned char reg2)
{    
    SPIFlashWriteEnable(1);  
    
    
    SPISendByte(0x01);
    SPISendByte(reg1);
    SPISendByte(reg2);
    

    SPIFlashWaitWhenBusy();
}

static void SPIFlashClearProtectForStatusReg(void)
{
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(1<<7);
    reg2 &= ~(1<<0);

    SPIFlashWriteStatusReg(reg1, reg2);
}


static void SPIFlashClearProtectForData(void)
{
    /* cmp=0,bp2,1,0=0b000 */
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(7<<2);
    reg2 &= ~(1<<6);

    SPIFlashWriteStatusReg(reg1, reg2);
}

/* erase 4K */
void SPIFlashEraseSector(unsigned int addr)
{
    SPIFlashWriteEnable(1);  

    
    SPISendByte(0x20);
    SpiFlashSendAddr(addr);
   

    SPIFlashWaitWhenBusy();
}

/* program */
void SPIFlashProgram(unsigned int addr, unsigned char *buf, int len)
{
    int i;
    
    SPIFlashWriteEnable(1);  

    
    SPISendByte(0x02);
    SpiFlashSendAddr(addr);

    for (i = 0; i < len; i++)
        SPISendByte(buf[i]);
    
    

    SPIFlashWaitWhenBusy();
    
}

void SPIFlashRead(unsigned int addr, unsigned char *buf, int len)
{
    int i;
    
    
    SPISendByte(0x03);
    SpiFlashSendAddr(addr);
    for (i = 0; i < len; i++)
        buf[i] = SPIReceByte();

}

void SPIFlashInit(void)
{
    SPIInit();
    SPIFlashClearProtectForStatusReg();
    SPIFlashClearProtectForData();
}


void read_spi_chip_id(void)
{
   int Mid,Did; 
   SpiFlashReadId(&Mid, &Did);
   printf("Manufacturer ID:%2x,Device ID:%2x\n\r",Mid,Did);
   
}

void do_erase_spi_flash(void)
{
    unsigned int addr;
	
	/* 获得地址 */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");

	SPIFlashEraseSector(addr);
	
}

void do_write_spi_flash(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");

	SPIFlashProgram(addr,str,strlen(str)+1);

}

void do_read_spi_flash(void)
{
	unsigned int addr;
	int i, j;
	unsigned char str[64];
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	addr = get_uint();
	SPIFlashRead(addr,str,64);

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


void spi_flash_test(void)
{
    
    char c;
    
    SPIFlashInit();
    

	while (1)
	{
	    /* 打印菜单, 供我们选择测试内容 */
	    printf("#####spi flash test#####\n\r");
		printf("[i] Read  spi flash id\n\r");
		printf("[e] Erase spi flash\n\r");
		printf("[w] Write spi flash\n\r");
		printf("[r] Read  spi flash\n\r");
		printf("[q] quit\n\r");
		printf("Enter selection: ");

		c = getc();
		printf("%c\n\r", c);

		switch (c)		 
		{
			case 'q':
			case 'Q':
				return;
				break;
				
			case 'i':
			case 'I':
				read_spi_chip_id();
				break;

			case 'e':
			case 'E':
			    do_erase_spi_flash();
				break;

			case 'w':
			case 'W':
				do_write_spi_flash();
				break;

			case 'r':
			case 'R':
				do_read_spi_flash();
				break;
			default:
				break;
		}
	}
}


