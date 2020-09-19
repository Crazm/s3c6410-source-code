
#include "framebuffer.h"
#include "lcd.h"


/*
 * http://blog.csdn.net/p1126500468/article/details/50428613
 */

 //-------------画圆函数。参数：圆心，半径，颜色----------	
 // 	   画1/8圆 然后其他7/8对称画  
 // 		 ---------------->X  
 // 		 |(0,0)   0  
 // 		 |	   7	 1	
 // 		 |	  6 	  2  
 // 		 |	   5	 3	
 // 	  (Y)V		  4  
 //  
 // 	 L = x^2 + y^2 - r^2 


void draw_circle(int x, int y, int r, unsigned int color)
{
    int a, b, num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)          // 1/8圆即可
    {
        put_pixel(x + a, y - b,color); // 0~1
        put_pixel(x - a, y - b,color); // 0~7
        put_pixel(x - a, y + b,color); // 4~5
        put_pixel(x + a, y + b,color); // 4~3
 
        put_pixel(x + b, y + a,color); // 2~3
        put_pixel(x + b, y - a,color); // 2~1
        put_pixel(x - b, y - a,color); // 6~7
        put_pixel(x - b, y + a,color); // 6~5
        
        a++;
        num = (a * a + b * b) - r*r;
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}
 
//-----------画线。参数：起始坐标，终点坐标，颜色--------
void draw_line(int x1,int y1,int x2,int y2,unsigned int color)
{
    int dx,dy,e;
    dx=x2-x1; 
    dy=y2-y1;
    if(dx>=0)
    {
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 1/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}   
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 2/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}   
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)
            if(dx>=dy) // 8/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}   
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 7/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}   
                    y1-=1;
                    e+=dx;
                }
            }
        }   
    }
    else //dx<0
    {
        dx=-dx;     //dx=abs(dx)
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 4/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}   
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 3/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}   
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)
            if(dx>=dy) // 5/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}   
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 6/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    put_pixel(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}   
                    y1-=1;
                    e+=dx;
                }
            }
        }   
    }
}
 
// 区域填色函数
// 参数：开始列数，结束列数，开始行数，结束行数，颜色
void fill_fb(int start_x, int end_x, int start_y, int end_y, unsigned int val)
{
    int i, j;
    for(i = start_y; i < end_y; i++)
    {
        for(j = start_x; j < end_x; j++)
        {
           put_pixel(i, j, val);
        }
    }
}

void clear_display(unsigned int color)
{
    unsigned int fb_base;
    int xres ,yres, bpp, x, y;

    /* 获得LCD的参数：fb_base ,xres,yres,bpp */
    get_lcd_params(&fb_base , &xres , &yres , &bpp);

    unsigned int *p = (unsigned int *)fb_base;
    /* 让LCD整屏输出制定颜色 */
    for(y = 0;y < yres ; y++)
      for(x = 0 ;x < xres ; x++)
          *p++ = color;  
}

void fb_disp_cross(unsigned int x,unsigned y,unsigned color)
{
    draw_line(x-10 , y , x+10, y, color);
    draw_line(x , y-10 , x, y+10, color);
}

