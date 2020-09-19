#include "../s3c6410.h"
#include "../include/stdio.h"
#include "adc.h"

void adc_test(void)
{
    int val,m,n;
    double vol;
    adc0_init();

    while(1)
    {
       val = adc0_read();
       vol =(double)val / 4095 * 3.3;
       m   = (int)vol;
       n   = (vol-m) * 1000;
       
       printf("adc:%d.%03dv\r",m,n);
       
    }
    
}
