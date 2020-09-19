#include "../s3c6410.h"
#include "../include/stdio.h"
#include "adc.h"
#include "../lcd/font.h"


void adc_test(void)
{
    int val,m,n;
    double vol;
    char str[32];

    while(1)
    {
       val = adc_read(1);
       vol =(double)val / 4095 * 3.3;
       m   = (int)vol;
       n   = (vol-m) * 1000;
       sprintf(str,"adc:%d.%03dv\r",m,n);
       fb_printf_string(30, 100,str,0xaabbcc);
    }
    
}
