#include "lcd.h"

static unsigned int fb_base;
static int xres,yres,bpp;

/* 获得LCD 参数 */
void fb_get_lcd_params(void)
{
    get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

/* 画点：默认24Bpp color 32位 */
void put_pixel(int x, int y, unsigned int color)
{
    unsigned int *p;
    p = (unsigned int *)(fb_base + y*xres*4 + x*4);
    *p = color;
}


