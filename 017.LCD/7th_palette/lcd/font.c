#include "framebuffer.h"
#include "lcd.h"

extern const unsigned char fontdata_8x16[];

void fb_printf_char(int x, int y,unsigned char c,unsigned char color)
{
    /* 根据c的ascii码在font_8*16中得到点阵 */
    unsigned char *dots = (unsigned char *)&fontdata_8x16[c*16];
    unsigned char data;

    /* 根据点阵来设置对应像素的颜色 */
    int i,j,bit;
    for(j = y; j < y+16; j++)
     {
        data = *dots++;
        bit  = 7;
        for(i = x; i < x+8; i++)
            {
            /* 根据点阵的某位决定是否描颜色 */
                if((data >>bit) & 0x1 )
                    put_pixel(i, j, color);
                bit--;
            }
     }   
}

void fb_printf_string(int x, int y,unsigned char* str,unsigned char color)
{
    int i=0;
    unsigned int fb_base;
    int xres,yres,bpp;
    get_lcd_params(&fb_base, &xres, &yres, &bpp);
    
    while(str[i])
      {
        if(str[i] == '\n')      y += 16;
        else if(str[i] == '\r') x = 0;
        else
            {
                fb_printf_char(x, y, str[i], color);
                x += 8;
                if(x >= xres) /* 换行 */
                    {
                        x = 0;
                        y += 16;
                    }
                
            }
        i++;
      }
}

