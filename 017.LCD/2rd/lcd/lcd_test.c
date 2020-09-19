#include "lcd.h"
#include "../my_printf.h"


void lcd_test(void)
{
    unsigned int fb_base;
    int xres ,yres, bpp, x, y;
    unsigned int *p;
    
    /* 初始化LCD */
    lcd_init();

    /* 使能LCD */
    lcd_enable();

    /* 获得LCD的参数：fb_base ,xres,yres,bpp */
    get_lcd_params(&fb_base , &xres , &yres , &bpp);
    printf("fb_base:%x ,xres:%d ,yres:%d,bpp:%d\n\r",fb_base,xres,yres,bpp);
    /* 往framebuffer里写数据 */
    if(bpp == 18 )
        {
           
            p = (unsigned int *)fb_base;
            /* 让LCD整屏输出绿色 */
            for(y = 0;y < yres ; x++)
                for(x = 0 ;x < yres ; x++)
                    *p++ = 0x00ff00; 
         
        }
    
}

