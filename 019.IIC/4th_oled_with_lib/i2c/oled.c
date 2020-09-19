#include "i2c_controller.h"
#include "oled_font.h"
#include "../include/stdio.h"
#include "../adc_touchscreen/adc.h"


int oled_write_data(unsigned char data)
{
    i2c_msg msg;
    int err;
    unsigned char buf[2];
  
    buf[0]    =   0x40;
    buf[1]    =   data;
    /* 构造i2c_msg */
    msg.addr  = (0x78>>1) ;
    msg.flags = 0;
    msg.len   = 2;
    msg.buf   = buf;
    msg.err   = 0;
    msg.cnt_transfered = -1;
    /* 调用i2c_transfer */
    err = i2c_transfer(&msg, 1);
    if(err)
      return err;
    return 0;
}

int oled_write_cmd(unsigned char cmd)
{
    i2c_msg msg;
    int err;
    unsigned char buf[2];
  
    buf[0]    =   0x00;
    buf[1]    =   cmd;
    /* 构造i2c_msg */
    msg.addr  = (0x78>>1) ;
    msg.flags = 0;
    msg.len   = 2;
    msg.buf   = buf;
    msg.err   = 0;
    msg.cnt_transfered = -1;
    /* 调用i2c_transfer */
    err = i2c_transfer(&msg, 1);
    if(err)
      return err;
    return 0;
}

void oled_clear(void)
{
    int i,j;
    for(i=0;i<8;i++)
      {
        oled_write_cmd(0xb0+i);     //设置页地址(显示哪一行)
        oled_write_cmd(0x00);       //设置设置显示的列地址的低四位
        oled_write_cmd(0x10);       //设置设置显示的列地址的高四位
        for(j=0;j<128;j++)
           oled_write_data(0);
      }
}

void oled_print_font(char row,char col,char *str)
{
    char j;
    
    while(*str)
    {
        oled_write_cmd(0xb0+row);                   //设置页地址(显示哪一行)
        oled_write_cmd(0x00+(col & 0xf));           //设置设置显示的列地址的低四位
        oled_write_cmd(0x10+((col>>4) & 0xf));     //设置设置显示的列地址的高四位
        for(j=0; j<8; j++)
            oled_write_data(F8X16[(*str - ' ')*16 +j]);
            
        oled_write_cmd(0xb0+row+1);                 //设置页地址(显示哪一行)
        oled_write_cmd(0x00+(col & 0xf));           //设置设置显示的列地址的低四位
        oled_write_cmd(0x10+((col>>4) & 0xf));     //设置设置显示的列地址的高四位
        for(j=0; j<8; j++)
            oled_write_data(F8X16[(*str - ' ')*16 +j+8]);
        str++;
        col += 8;
        if(col > 127)
        {
          row += 2;
          col = 0;
        }
    }
}

void oled_init(void)
{
    
    oled_write_cmd(0xae);	//--关闭oled显示

    oled_write_cmd(0x00);	//--设置列低四位起始地址
    oled_write_cmd(0x10);	//--设置列高四位起始地址

    oled_write_cmd(0x40);	//--set start line address

    oled_write_cmd(0xb0);	//--set page address

    oled_write_cmd(0x81);	//--set contrast control register
    oled_write_cmd(0x66);

    oled_write_cmd(0xa1);	//--set segment re-map 127 to 0   a0:0 to seg127
    oled_write_cmd(0xa6);	//--set normal display

    oled_write_cmd(0xc9);	//--set com(N-1)to com0  c0:com0 to com(N-1)

    oled_write_cmd(0xa8);	//--set multiples ratio(1to64)
    oled_write_cmd(0x3f);	//--1/64 duty

    oled_write_cmd(0xd3);	//--set display offset
    oled_write_cmd(0x00);	//--not offset

    oled_write_cmd(0xd5);	//--set display clock divide ratio/oscillator frequency
    oled_write_cmd(0xf0);	//--set divide ratio

    oled_write_cmd(0xd9);	//--set pre-charge period
    oled_write_cmd(0xf1);

    oled_write_cmd(0xda);	//--set com pins hardware configuration
    oled_write_cmd(0x12);

    oled_write_cmd(0xdb);	//--set vcomh
    oled_write_cmd(0x40);

    oled_write_cmd(0x8d);	//--set chare pump enable/disable
    oled_write_cmd(0x14);	//--set(0x10) disable
    
    oled_write_cmd(0xaf);	//--turn on oled panel
	
	oled_clear();
}

void oled_test(void)
{
    
    i2c_init();
    
    oled_init();

    int val,m,n;
    double vol;
    char str[16];
    
    adc0_init();

    oled_print_font(0,0,"Hello world!");
    oled_print_font(2,0,"By Crazm");
    while(1)
    {
       val = adc0_read();
       vol =(double)val / 4095 * 3.3;
       m   = (int)vol;
       n   = (vol-m) * 1000;
       sprintf(str,"adc: %1d.%3dV",m,n);
       oled_print_font(4,0,str); 
    }
}

