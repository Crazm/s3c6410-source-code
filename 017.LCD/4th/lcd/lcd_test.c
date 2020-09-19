#include "lcd.h"
#include "../my_printf.h"
#include "geometry.h"
#include "framebuffer.h"
#include "font.h"

extern void delay(volatile unsigned int t);


void lcd_test(void)
{
    unsigned int fb_base;
    int xres ,yres, bpp, x, y;
    unsigned int *p;
    
    /* 初始化LCD */
    lcd_init();


    /* 获得LCD的参数：fb_base ,xres,yres,bpp */
    get_lcd_params(&fb_base , &xres , &yres , &bpp);
    printf("fb_base:%x ,xres:%d ,yres:%d,bpp:%d\n\r",fb_base,xres,yres,bpp);
    fb_get_lcd_params();
    
    /* 往framebuffer里写数据 */
    if(bpp == 24 )
        {
            p = (unsigned int *)fb_base;
            /* 让LCD整屏输出红色 */
            for(y = 0;y < yres ; y++)
                for(x = 0 ;x < xres ; x++)
                    *p++ = 0xff0000;
            delay(1000000);

            p = (unsigned int *)fb_base;
            /* 让LCD整屏输出绿色 */
            for(y = 0;y < yres ; y++)
                for(x = 0 ;x < xres ; x++)
                    *p++ = 0x00ff00;
            delay(1000000);

            p = (unsigned int *)fb_base;
            /* 让LCD整屏输出蓝色 */
            for(y = 0;y < yres ; y++)
                for(x = 0 ;x < xres ; x++)
                    *p++ = 0x0000ff;
            delay(1000000);

            /* 让LCD整屏输出黑色 */
            clr_fb_b();
                
            /*画线*/
            draw_line(0,0, xres-1, 0, 0xff0000);
            draw_line(0,0,0,yres-1, 0xff0000);
            draw_line(0,yres-1, xres-1, yres-1, 0xff0000);
            draw_line(xres-1,0, xres-1, yres-1, 0xff0000);
            draw_line(0,0, xres-1, yres-1, 0xff0000);
            draw_line(xres-1 , 0, 0,yres-1, 0xff0000);
            delay(1000000);

            /* 画线 */
            draw_circle(xres/2, yres/2, yres/4, 0x00f0f0);
            delay(1000000);

            /* 写字 */
            clr_fb_b();
            char * character = "This is my designed ! \nMy name is sgf!";
            fb_printf_string(22, 22, character, 0xff0000);
            fb_printf_char(400, 200, 'A', 0x123456);
            
        }
    
}

