#include "lcd.h"

static unsigned int fb_base;
static int xres,yres,bpp;

/* 获得LCD 参数 */
void fb_get_lcd_params(void)
{
    get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

/* 画点：默认8Bpp color */
void put_pixel(int x, int y, unsigned char color)
{
    unsigned char *p;
    p = (unsigned char *)(fb_base + y*xres + x);
    *p = color;
}


