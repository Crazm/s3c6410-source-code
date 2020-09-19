#include "../s3c6410.h"
#include "../timer.h"
#include "../include/stdio.h"
#include "DS18B20.h"

/* 使用GPN8作为data口 */

/* rom commands */
#define SEARCH_ROM    0xF0
#define READ_ROM      0x33
#define MATCH_ROM     0x55
#define SKIP_ROM      0xCC
#define ALARM_ROM     0xEC

/* functions commands */
#define CONVERT_TEAMPERATURE 0x44
#define WRITE_SCRATCHPAD     0x4E
#define READ_SCRATCHPAD      0xBE
#define COPY_SCRATCHPAD      0x48
#define RECALL_EEPROM        0xB8
#define READ_POWER_SUPPLY    0xB4



static void set_io_as_input(void)
{
    GPNCON_REG &= ~(3<<16);
}

static void set_io_as_output(void)
{
    GPNCON_REG &= ~(3<<16);
    GPNCON_REG |=  (1<<16); 
}

static void set_io_pull_up(void)
{
    GPNPUD_REG |= (2<<16);
}

static void set_data(char val)
{
    
    if(val)
        GPNDAT_REG |= (1<<8);
    else
        GPNDAT_REG &= ~(1<<8); 
}

static char get_data(void)
{
    if(GPNDAT_REG & (1<<8))
         return 1;
    else
         return 0; 
}


static char wait_for_updown(char val,int us)
{
   while(us--)
   {
      if(get_data() == val)
        return 0;
      udelay(1);
   }
   return -1;
}


static char ds18b20_init(void)
{
    set_io_as_output();
    set_io_pull_up();
    set_data(0);
    udelay(500);
    set_io_as_input();
    if(wait_for_updown(0 , 480))
     {
        printf("init:wait for down err!\n\r");
        return -1;
     }
    //set_io_as_output();
    //set_data(1);
    udelay(400); 
    return 0;
}

static void ds18b20_write_byte(char val)
{
    int i;
    for(i=0; i<8; i++)
    {
       set_io_as_output();
       set_data(0);
       udelay(1);
       set_data(val & 0x1);
       udelay(60);
       val >>= 1;
       set_data(1);
       udelay(1);
    } 
    set_io_as_input();  //释放总线
}

static char ds18b20_read_byte(void)
{
    int i;
    char temp = 0;
    for(i=0; i<8; i++)
    {
       set_io_as_output();
       set_data(0);
       udelay(2);
       set_io_as_input();
       udelay(8);
       if(get_data())
         temp |= (1<<i);
       udelay(5);
       set_data(1);
       udelay(60);
    }
    set_io_as_input();  //释放总线
    return temp; 
}

static void ds18b20_write_command(char val)
{
    ds18b20_write_byte(val);
}

static char CRC8_Table(unsigned char *p, char counter)
{
   char crc8 = 0;
   for( ; counter > 0; counter--)
    {
       crc8 = CRC8Table[crc8^*p]; //查表得到CRC码
       p++;
    }
  return crc8;
}


/* DS18B20实际操作函数 */
void ds18b20_read_rom(void)
{
    char rom[8],crc;
    int i;
    if(ds18b20_init())      return;
    ds18b20_write_command(READ_ROM); /* 读ROM */
    for(i=0; i<8; i++)
      rom[i] = ds18b20_read_byte();

    crc = CRC8_Table(rom, 7);
    if(crc != rom[7])   printf("ROM CRC err!\n\r");
    printf("DS18B20 ROM:%02x,%02x %02x %02x %02x %02x %02x,%02x\n\r",\
    rom[0],rom[1],rom[2],rom[3],rom[4],rom[5],rom[6],rom[7]);
}

static char ds18b20_start_convert(void)
{
    if(ds18b20_init())      return -1;
    ds18b20_write_command(SKIP_ROM);    /* 跳过ROM */
    ds18b20_write_command(CONVERT_TEAMPERATURE); /* 开始转换温度 */
    udelay(750000);
    if(wait_for_updown(1,1000000))  /* 等待转换完成 */
     {
        printf("wait for convert err!\n\r");
        return -1;
     }
    return 0;
}

static char ds18b20_read_ram(char *ram)
{
    int i;
    char crc;
    if(ds18b20_init())      return -1;
    ds18b20_write_command(SKIP_ROM);    /* 跳过ROM */
    ds18b20_write_command(READ_SCRATCHPAD); /* 读取RAM */
    for(i=0; i<9; i++)
       ram[i] = ds18b20_read_byte();
    crc = CRC8_Table(ram, 8);
    if(crc != ram[8])  
    {
        printf("RAM CRC err!\n\r");
        return -1;
    }
    return 0;
    
}

static char ds18b20_write_ram(char *ram)
{
    int i;
    if(ds18b20_init())      return -1;
    ds18b20_write_command(SKIP_ROM);    /* 跳过ROM */
    ds18b20_write_command(WRITE_SCRATCHPAD); /* 写RAM */
    for(i=0; i<3; i++)
       ds18b20_write_byte(ram[i]);
    return 0;
}

static char ds18b20_set_precision(int prec)
{
    char ram[3];
    if(prec == 9)
        ram[2] = 0x1f;  //9bit
    else if(prec == 10)
        ram[2] = 0x3f;  //10bit
    else if(prec == 11)
        ram[2] = 0x5f;  //11bit
    else 
        ram[2] = 0x7f;  //12bit
    if(ds18b20_write_ram(ram))
     {
        printf(" WRITE RAM err!\n\r");
        return -1;
     }
     
    if(ds18b20_init())      return -1;
    ds18b20_write_command(SKIP_ROM);    /* 跳过ROM */
    ds18b20_write_command(COPY_SCRATCHPAD); /* 把RAM复制到EEPROM */
    if(wait_for_updown(1,2000000))  /* 等待复制完成 */
     {
        printf("wait for write eeprom err!\n\r");
        return -1;
     }
    return 0;
    
}

static char ds18b20_read_temperature(double *temp)
{
    unsigned char ram[9];
    double val[] = {0.0625, 0.125, 0.25, 0.5, 1, 2, 4, 8, 16, 32, 64};
    double sum = 0;
	int i;
	
	if (ds18b20_start_convert())
		return -1;

	if (ds18b20_read_ram(ram))
		return -1;

	/* 计算温度:先判断精度 */
	if ((ram[4] & (3<<5)) == 0) /* 精度: 9bit */
		i = 3;
	else if ((ram[4] & (3<<5)) == (1<<5)) /* 精度: 10bit */
		i = 2;
	else if ((ram[4] & (3<<5)) == (2<<5)) /* 精度: 11bit */
		i = 1;
	else
		/* 精度是 12 bit */
		i = 0;
	
	for (; i < 8; i++)
	{
		if (ram[0] & (1<<i))
			sum += val[i];
	}

	for (i = 0; i < 3; i++)
	{
		if (ram[1] & (1<<i))
			sum += val[8+i];
	}

	if (ram[1] & (1<<3))
		sum = 0 - sum;

	*temp = sum;
	return 0;
}

void ds18b20_test(void)
{
	double temp;
	int m,n,prec;
	set_io_as_input();  /* 释放总线 */
    ds18b20_read_rom();
    printf("please input precision(9-12):");
    prec = get_int();
    ds18b20_set_precision(prec);
    while(1)
    {
        if (0 == ds18b20_read_temperature(&temp))
		{
			m = (int)temp;	/* 3.01, m = 3 */
			temp = temp - m;	/* 小数部分: 0.01 */
			n = temp * 10000;  /* 10 */
			
			/* 在串口上打印 */
			printf("ds18b20 temperature: %d.%04d\n\r", m, n);  /* 3.010v */
		}
    } 
    
}


