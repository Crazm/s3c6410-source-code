#ifndef _TSLIB_H
#define _TSLIB_H

int ts_read(int *lcd_x, int *lcd_y, int *lcd_pressure);
void ts_calibrate(void);

#endif /* _TSLIB_H */
