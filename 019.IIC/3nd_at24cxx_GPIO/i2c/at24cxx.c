#include "../include/stdio.h"
#include "../timer.h"
#include "../s3c6410.h"


/* SDA:GPB3 SCL:GPB2 */

static void set_sda_as_input(void)
{
    GPBCON_REG &= ~(15<<24);
}

static void set_sda_as_output(void)
{
    GPBCON_REG &= ~(15<<24);
    GPBCON_REG |=  (1<<24);
}

static void set_scl_as_output(void)
{
    GPBCON_REG &= ~(15<<20);
    GPBCON_REG |=  (1<<20);
}

static void set_sda_data(char val)
{
    
    if(val)
        GPBDAT_REG |= (1<<6);
    else
        GPBDAT_REG &= ~(1<<6);
}

static void set_scl_data(char val)
{
    
    if(val)
        GPBDAT_REG |= (1<<5);
    else
        GPBDAT_REG &= ~(1<<5);
}


static char get_sda_data(void)
{
    if(GPBDAT_REG & (1<<6))
        return 1;
    else
        return 0;
}

static char wait_for_ud(char val,  int us)
{
    while(us--)
    {
        if(get_sda_data() == val)
            return 0;
        udelay(1);
    }
    return -1;
}

static void at24cxx_start(void)
{
    set_sda_as_output();
    set_scl_data(1);
    set_sda_data(1);
    udelay(6);
    set_sda_data(0);
    udelay(8);
}

static void at24cxx_end(void)
{
    set_sda_as_output();
    set_sda_data(0);
    udelay(3);
    set_scl_data(1);
    udelay(8);
    set_sda_data(1);
    udelay(8);
}

static char at24cxx_write_byte(unsigned char data)
{
    int i;
    set_sda_as_output();
    for(i=0; i<8; i++)
     {
        set_scl_data(0);
        udelay(1);
        set_sda_data(data & 0x80);
        data <<= 1;
        udelay(7);
        set_scl_data(1);
        udelay(8);
     }
     set_scl_data(0);
     set_sda_as_input();
     udelay(8);
     set_scl_data(1);
     if(wait_for_ud(0, 100000))
        {
            printf("no ack!\n\r");
            return -1;
        }
     udelay(7);
     set_scl_data(0);
     udelay(2);
     set_sda_as_output();
     set_sda_data(1);
     udelay(3);
     return 0;
     
}

static char at24cxx_read_byte(unsigned char ack_sign)
{
    int i;
    char temp = 0;
    set_sda_as_input();
    for(i=0; i<8; i++)
     {
        set_scl_data(0);
        udelay(8);
        set_scl_data(1);
        udelay(5);
        if(get_sda_data())
            temp |= ( 1<<(7-i));
        udelay(3);
     }
     set_scl_data(0);
     udelay(4);
     set_sda_as_output();
     if(ack_sign == 1)
         set_sda_data(0);
     else
         set_sda_data(1);
     udelay(4);
     set_scl_data(1);
     udelay(8);
     set_scl_data(0);
     udelay(2);
     set_sda_as_output();
     set_sda_data(1);
     udelay(3);
     return temp;   
}


int at24cxx_write(unsigned char addr,unsigned char *data,int len)
{
    int i;
    for(i=0; i<len; i++)
    {
      at24cxx_start();
      if(at24cxx_write_byte(0xa0))
        return -1;
      if(at24cxx_write_byte(addr++))
        return -1;
      if(at24cxx_write_byte(data[i]))
        return -1; 
      at24cxx_end();
      udelay(10000);
    }
    
    return 0;
}

int at24cxx_read(unsigned char addr,unsigned char *data,int len)
{
    int i,sign = 1;
    at24cxx_start();
    if(at24cxx_write_byte(0xa0))
        return -1;
    if(at24cxx_write_byte(addr))
        return -1;
    at24cxx_end();
    udelay(600);
    at24cxx_start();
    if(at24cxx_write_byte(0xa1))
        return -1;
    for(i=0; i<len; i++)
    {
      if(i == (len - 1)) sign = 0;
      data[i] = at24cxx_read_byte(sign);
    }
    at24cxx_end();
    return 0;
}

void i2c_init(void)
{

    set_scl_as_output();
    set_sda_as_output();
    GPBPUD_REG = (2<<10)|(2<<12);     //使能上拉
    set_scl_data(1);
    set_sda_data(1);
    
}


