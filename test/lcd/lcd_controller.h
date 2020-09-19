#ifndef _LCD_CONTROLLER_H
#define _LCD_CONTROLLER_H

#include "lcd.h"

typedef struct lcd_controller {
	char *name;
	void (*init)(p_lcd_params plcdparams);
	void (*enable)(void);
	void (*disable)(void);
}lcd_controller, *p_lcd_controller;

extern int register_lcd_controller(p_lcd_controller plcdcon);
extern void lcd_contoller_add(void);
extern int lcd_controller_init(p_lcd_params plcdparams);
extern int lcd_controller_enable(p_lcd_params plcdparams);
extern int lcd_controller_disable(p_lcd_params plcdparams);



#endif /* _LCD_CONTROLLER_H */

