#include "touchscreen.h"
#include "../lcd/geometry.h"
#include "../lcd/framebuffer.h"
#include "../lcd/font.h"
#include "../include/stdio.h"
#include "tslib.h"

void touchscreen_test(void)
{
    clear_display(0);

    int x, y, pressure;

    touchscreen_init();
    
   /* 显示文字提示较准 */
	fb_printf_string(70, 70, "Touch cross to calibrate touchscreen", 0xffffff);
	ts_calibrate();

	/* 显示文字提示绘画 */
	fb_printf_string(70, 272 - 70, "OK! To draw!", 0xffffff);

	while (1)
	{
		if (ts_read(&x, &y, &pressure) == 0)
		{
			printf(" x = %d, y = %d\n\r", x, y);

			if (pressure)
			{
				put_pixel(x, y, 0xff00);
			}
		}
	}
    
}


