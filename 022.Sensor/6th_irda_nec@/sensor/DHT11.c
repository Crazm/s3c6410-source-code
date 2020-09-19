#include "../s3c6410.h"
#include "../timer.h"
#include "../include/stdio.h"

static void set_io_as_input(void)
{
    GPNCON_REG &= ~(3<<16);
}

static void set_io_as_output(void)
{
    GPNCON_REG &= ~(3<<16);
    GPNCON_REG |=  (1<<16); 
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


void dht11_start(void)
{
    set_io_as_output();
    set_data(0);
    mdelay(20);
    set_io_as_input(); 
}

static char dht11_wait_ack(void)
{
	udelay(60);
	return get_data();
}


static char dht11_recv_byte(void)
{
    int i;
    unsigned char dat = 0;
    for(i=0; i<8; i++)
    {
       if(wait_for_updown(1, 1000)) 
        {
          printf("2wait for up err!\n\r");
          return -1;
        }
        
       udelay(40);
       dat <<= 1;
       
       if(get_data() == 1)
         dat |= 1;
         
       if(wait_for_updown(0, 1000)) 
        {
          printf("2wait for down err!\n\r");
          return -1;
        }  
    }
    return dat;
}

char dht11_read(char *hum,char *temp)
{
    unsigned char hum_m,hum_n;
    unsigned char temp_m,temp_n;
    unsigned char check;
    dht11_start();
    if (dht11_wait_ack() !=0 )
	{
		printf("dht11 not ack, err!\n\r");
		return -1;
	}
    if(wait_for_updown(0, 1000))
       {
         printf("1wait for down err!\n\r");
         return -1;
       }
    if(wait_for_updown(1, 1000))
       {
         printf("1wait for up err!\n\r");
         return -1;
       }
    hum_m  = dht11_recv_byte();
    hum_n  = dht11_recv_byte();
    temp_m = dht11_recv_byte();
    temp_n = dht11_recv_byte();
    check  = dht11_recv_byte();
    set_io_as_output();
    set_data(1);
    if( (hum_m+hum_n+temp_m+temp_n) == check)
    {
       *hum  = hum_m;
		*temp = temp_m;
		mdelay(2000);  /* 读取周期是2S, 不能读太频繁 */
		return 0; 
    }
    printf("dht11 checksum err!\n\r");
    return -1;
}

void dht11_init(void)
{
    set_io_as_output();
    GPNPUD_REG |= (2<<16);
    set_data(1);
    mdelay(1000);
}

void dht11_test(void)
{
   
    dht11_init();
    char hum,temp;
    while(1)
    {
      if (dht11_read(&hum, &temp) !=0 )
		{
			printf("\n\rdht11 read err!\n\r");
			dht11_init();
		}
		else
		{
			printf("\n\rDHT11 : %d humidity, %d temperature\n\r", hum, temp);
		}
      
    }   
}

