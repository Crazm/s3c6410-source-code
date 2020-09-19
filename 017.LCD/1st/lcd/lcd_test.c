

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

    /* 往framebuffer里写数据 */
    if(bpp == 18 )
        {
            p = (unsigned int *)fb_base;
            /* 让LCD整屏输出红色 */
            for(x = 0;x < xres ; x++)
                for(y = 0 ;y < yres ; y++)
                    *p++ = 0xff0000;    
            
        }
        
    
    
}

